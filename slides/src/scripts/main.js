var isWebKit = 'webkitAppearance' in document.documentElement.style,
  // zoom-based scaling causes font sizes and line heights to be calculated differently
  // on the other hand, zoom-based scaling correctly anti-aliases fonts during transforms (no need for layer creation hack)
  scaleMethod = isWebKit ? 'zoom' : 'transform',
  bespoke = require('bespoke'),
  bullets = require('bespoke-bullets'),
  classes = require('bespoke-classes'),
  cursor = require('bespoke-cursor'),
  fullscreen = require('bespoke-fullscreen'),
  hash = require('bespoke-hash'),
  nav = require('bespoke-nav'),
  onstage = require('bespoke-onstage'),
  overview = require('bespoke-overview'),
  title = require('bespoke-title'),
  scale = require('bespoke-scale'),
  slidenumber = require('bespoke-slidenumber'),
  progress = require('bespoke-progress'),
  blackout = require('bespoke-blackout');

function prismjs() {
  var Prism = require('prismjs');
  require('prismjs/components/prism-c');
  require('prismjs/components/prism-cpp');
  require('prismjs/components/prism-csharp');
  require('prismjs/components/prism-nasm');
  require('prismjs/plugins/toolbar/prism-toolbar');
  require('prismjs/plugins/show-language/prism-show-language');
  require('prismjs/plugins/keep-markup/prism-keep-markup');
  return function() {
    Prism.highlightAll();
  }
}

bespoke.from({ parent: 'article.deck', slides: 'section' }, [
  classes(),
  cursor(),
  nav(),
  fullscreen(),
  (scaleMethod ? scale(scaleMethod) : function(deck) { }),
  overview({ columns: 3 }),
  bullets('.build, .build-items > *:not(.build-items)'),
  hash(),
  progress(),
  title(),
  prismjs(),
  onstage(),
  slidenumber('/'),
  blackout(),
]);
