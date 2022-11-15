simulador_sources = [
  'main.c',
  'simulador-window.c',
  'simulador-about.h',
  'simulador-about.c',
  'util/parser.h',
  'util/parser.c',
  'util/lexer.h',
  'util/lexer.c',
  'util/cfg.h',
  'util/cfg.c',
  'util/token.h',
  'util/token.c',
  'util/production.h',
  'util/production.c',
  'util/symbol.h',
  'util/symbol.c',
  'util/instruction.h',
  'util/instruction.c',
  'util/program_context.h',
  'util/program_context.c',
]

simulador_deps = [
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('gtk+-3.0', version: '>= 3.22'),
  dependency('gtksourceview-4', version: '>= 4.8'),
]

gnome = import('gnome')

simulador_sources += gnome.compile_resources('simulador-resources',
  'simulador.gresource.xml',
  c_name: 'simulador'
)

executable('simulador', simulador_sources,
  dependencies: simulador_deps,
  install: true,
)
