"""Exercise release integrity gates using synthetic archives, not Windows binaries."""
import hashlib
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
import zipfile

SCRIPT = Path(__file__).with_name('prepare_release.py')

class ReleasePackagingTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        (self.root / 'release/licenses').mkdir(parents=True)
        (self.root / 'review').mkdir()
        (self.root / 'tested').mkdir()
        shutil.copy2(SCRIPT, self.root / 'release/prepare_release.py')
        for name in ('THIRD-PARTY-NOTICES.md', 'notes.zh-CN.md'):
            (self.root / 'release' / name).write_text('release fixture')
        (self.root / 'release/licenses/fixture.txt').write_text('license fixture')
        (self.root / 'review/HARDWARE-TEST.md').write_text('current guide')
        self.files = {name: b'fixture:' + name.encode() for name in (
            'AirPodsDesktop.exe', 'Qt6Core.dll', 'Qt6Gui.dll', 'Qt6Widgets.dll',
            'platforms/qwindows.dll', 'multimedia/ffmpegmediaplugin.dll')}

    def archive(self, extra=None, bad_checksum=False):
        archive = self.root / 'tested/tested.zip'
        with zipfile.ZipFile(archive, 'w') as target:
            for name, data in (self.files | (extra or {})).items():
                target.writestr(name.replace('/', '\\'), data)
        digest = '0' * 64 if bad_checksum else hashlib.sha256(archive.read_bytes()).hexdigest()
        Path(str(archive) + '.sha256').write_text(digest + '  tested.zip', encoding='utf-8-sig')

    def run_package(self):
        return subprocess.run([sys.executable, str(self.root / 'release/prepare_release.py')],
            env=os.environ | {'TESTED_COMMIT': 'fixture-commit', 'TESTED_RUN': 'fixture-run',
                              'GITHUB_REF_NAME': 'v0.6.1-pro3.1'},
            capture_output=True, text=True)

    def test_preserves_every_binary_and_adds_notices(self):
        self.archive()
        result = self.run_package()
        self.assertEqual(result.returncode, 0, result.stderr)
        package = next((self.root / 'dist').glob('*.zip'))
        with zipfile.ZipFile(package) as z:
            for name, data in self.files.items():
                self.assertEqual(z.read(name), data)
            self.assertIn('BUILD-PROVENANCE.json', z.namelist())
            self.assertIn('licenses/fixture.txt', z.namelist())
            self.assertEqual(z.read('HARDWARE-TEST.md'), b'current guide')
        digest = (self.root / 'dist/SHA256SUMS.txt').read_text().split()[0]
        self.assertEqual(digest, hashlib.sha256(package.read_bytes()).hexdigest())

    def test_rejects_mismatched_checksum(self):
        self.archive(bad_checksum=True)
        self.assertNotEqual(self.run_package().returncode, 0)
        self.assertFalse((self.root / 'dist').exists())

    def test_rejects_archive_path_traversal(self):
        self.archive({'../escaped.txt': b'unsafe'})
        self.assertNotEqual(self.run_package().returncode, 0)
        self.assertFalse((self.root / 'escaped.txt').exists())

    def test_rejects_missing_runtime(self):
        del self.files['platforms/qwindows.dll']
        self.archive()
        self.assertNotEqual(self.run_package().returncode, 0)
        self.assertFalse((self.root / 'dist').exists())

if __name__ == '__main__':
    unittest.main()
