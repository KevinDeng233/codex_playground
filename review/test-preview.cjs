// Run with jsdom 26.1.0 available on NODE_PATH. Offline DOM interaction tests.
const fs = require('node:fs');
const path = require('node:path');
const assert = require('node:assert/strict');
const { JSDOM, VirtualConsole } = require('jsdom');
const errors = [];
const console = new VirtualConsole();
console.on('jsdomError', error => errors.push(error));
const dom = new JSDOM(fs.readFileSync(path.join(__dirname, 'preview.html'), 'utf8'), {
  runScripts: 'dangerously', virtualConsole: console
});
const document = dom.window.document;
const $ = id => document.getElementById(id);
let checks = 0;
function check(fn) { fn(); checks++; }
check(() => assert.equal($('title').textContent, 'Kevin 的 AirPods Pro'));
check(() => assert.equal(document.querySelectorAll('.reading').length, 3));
for (const [scenario, expected] of Object.entries({
  ready: ['80%', '60%', '40%'], waiting: ['—', '—', '—'],
  disconnected: ['—', '—', '—'], partial: ['80%', '60%', '—'],
  low: ['10% ⚡', '60%', '40%'], scanner: ['—', '—', '—']
})) {
  $('scenario').value = scenario;
  $('scenario').dispatchEvent(new dom.window.Event('change'));
  check(() => assert.deepEqual([...document.querySelectorAll('.reading')].map(e => e.textContent), expected));
  check(() => assert.equal($('popup').hidden, false));
}
$('name').value = '<img src=x onerror=alert(1)>';
$('name').dispatchEvent(new dom.window.Event('input'));
check(() => assert.equal($('title').textContent, '<img src=x onerror=alert(1)>'));
check(() => assert.equal($('title').children.length, 0));
$('close').click();
check(() => assert.equal($('popup').hidden, true));
$('reopen').click();
check(() => assert.equal($('popup').hidden, false));
$('theme').click();
check(() => assert($('popup').classList.contains('light')));
$('show').click();
check(() => assert.equal($('popup').hidden, false));
document.dispatchEvent(new dom.window.KeyboardEvent('keydown', { key: 'Escape' }));
check(() => assert.equal($('popup').hidden, true));
const image = document.querySelector('.device');
check(() => assert(fs.existsSync(path.resolve(__dirname, image.getAttribute('src')))));
check(() => assert.deepEqual(errors, []));
process.stdout.write(`PASS: ${checks} offline preview checks\n`);
dom.window.close();
