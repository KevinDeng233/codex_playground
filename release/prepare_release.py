"""Verify the tested artifact, preserve binaries, and add release documentation."""
import hashlib
import json
import os
from pathlib import Path
import shutil
import zipfile

root = Path(__file__).resolve().parent.parent
archives = list((root / "tested").glob("*.zip"))
if len(archives) != 1:
    raise RuntimeError("Expected exactly one tested application archive")
archive = archives[0]
checksum_file = Path(str(archive) + ".sha256")
expected = checksum_file.read_text(encoding="utf-8-sig").split()[0].lower()
if hashlib.sha256(archive.read_bytes()).hexdigest() != expected:
    raise RuntimeError("The tested artifact checksum did not match")

staging = root / "release-staging"
staging.mkdir(exist_ok=False)
with zipfile.ZipFile(archive) as source:
    for member in source.infolist():
        # Windows-created ZIP names can use backslashes. Normalize before checking paths.
        relative = Path(member.filename.replace("\\", "/"))
        if relative.is_absolute() or ".." in relative.parts or ":" in member.filename:
            raise RuntimeError("Unsafe archive entry")
        if member.is_dir():
            continue
        dest = staging / relative
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_bytes(source.read(member))

required = ["AirPodsDesktop.exe", "Qt6Core.dll", "Qt6Widgets.dll", "Qt6Gui.dll", "platforms/qwindows.dll", "multimedia/ffmpegmediaplugin.dll"]
for filename in required:
    if not (staging / filename).is_file():
        raise RuntimeError(f"Missing runtime file: {filename}")
original = {
    str(p.relative_to(staging)): hashlib.sha256(p.read_bytes()).hexdigest()
    for p in staging.rglob("*") if p.is_file() and p.suffix.lower() in (".exe", ".dll")
}
shutil.copytree(root / "release/licenses", staging / "licenses", dirs_exist_ok=True)
for name in ("THIRD-PARTY-NOTICES.md", "notes.zh-CN.md"):
    shutil.copy2(root / "release" / name, staging / name)
shutil.copy2(root / "review/HARDWARE-TEST.md", staging / "HARDWARE-TEST.md")
(staging / "BUILD-PROVENANCE.json").write_text(json.dumps({
    "tested_commit": os.environ["TESTED_COMMIT"],
    "tested_run": os.environ["TESTED_RUN"],
    "tested_archive_sha256": expected,
    "release_tag": os.environ["GITHUB_REF_NAME"],
    "binary_sha256": original,
}, indent=2) + "\n", encoding="utf-8")
for filename, digest in original.items():
    if hashlib.sha256((staging / filename).read_bytes()).hexdigest() != digest:
        raise RuntimeError(f"A tested binary was modified: {filename}")
dist = root / "dist"
dist.mkdir(exist_ok=False)
name = f"AirPodsDesktop-{os.environ['GITHUB_REF_NAME']}-win11-x64-portable.zip"
with zipfile.ZipFile(dist / name, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=6) as target:
    for p in sorted(staging.rglob("*")):
        if p.is_file():
            target.write(p, p.relative_to(staging).as_posix())
digest = hashlib.sha256((dist / name).read_bytes()).hexdigest()
(dist / "SHA256SUMS.txt").write_text(f"{digest}  {name}\n", encoding="ascii")
print(f"Verified {len(original)} unchanged binaries; prepared {name} ({digest})")
