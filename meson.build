project(
  'autoclicker',
  'cpp',
  version: '1.0.0',
  meson_version: '>=1.7.0',
  default_options: ['cpp_std=c++23'],
)

if host_machine.system() != 'windows'
  error('autoclicker currently supports only Windows builds.')
endif

cpp = meson.get_compiler('cpp')
qmake = find_program('qmake6', 'qmake', required: true)

qt6_dep = dependency(
  'qt6',
  modules: ['Core', 'Gui', 'Quick', 'Qml'],
  method: 'config-tool',
  main: true,
)
user32_dep = cpp.find_library('user32', required: true)

src_inc = include_directories('src')
models_inc = include_directories('src/models')
project_includes = [src_inc, models_inc]
qt_bins_dir = run_command(qmake, '-query', 'QT_INSTALL_BINS', check: true).stdout().strip()
qt_headers_dir = run_command(qmake, '-query', 'QT_INSTALL_HEADERS', check: true).stdout().strip()
project_include_args = [
  '-I' + meson.project_source_root() / 'src',
  '-I' + meson.project_source_root() / 'src/models',
  '-I' + qt_headers_dir,
]

cpp_sources = files(
  'src/main.cpp',
  'src/autoclickercontroller.cpp',
  'src/clicker.cpp',
  'src/keybinder.cpp',
  'src/models/clickprofile.cpp',
  'src/translationmanager.cpp',
)

qt_moc_headers = files(
  'src/autoclickercontroller.h',
  'src/clicker.h',
  'src/keybinder.h',
  'src/models/clickprofile.h',
  'src/translationmanager.h',
)

moc = find_program(qt_bins_dir / 'moc.exe', required: true)
rcc = find_program(qt_bins_dir / 'rcc.exe', required: true)
windeployqt = find_program(qt_bins_dir / 'windeployqt.exe', required: true)
pwsh = find_program('pwsh', 'powershell', required: true)

moc_gen = generator(
  moc,
  output: 'moc_@BASENAME@.cpp',
  arguments: ['@INPUT@', '-o', '@OUTPUT@'] + project_include_args,
)
moc_sources = moc_gen.process(qt_moc_headers)

qml_resources = custom_target(
  'qml_resources',
  input: 'resources/qml.qrc',
  output: 'qrc_qml.cpp',
  command: [rcc, '-name', 'qml', '-o', '@OUTPUT@', '@INPUT@'],
  depend_files: files('qml/main.qml', 'qml/ClickProfileItem.qml'),
)

autoclicker_exe = executable(
  'autoclicker',
  sources: [
    cpp_sources,
    moc_sources,
    qml_resources,
  ],
  include_directories: project_includes,
  dependencies: [qt6_dep, user32_dep],
  win_subsystem: 'windows',
  install: true,
)

deploy_mode = get_option('buildtype') == 'debug' ? 'debug' : 'release'
build_deploy_dir = meson.current_build_dir() / 'deploy'

custom_target(
  'autoclicker_deploy',
  output: 'autoclicker.deploy.stamp',
  build_by_default: true,
  depends: autoclicker_exe,
  command: [
    pwsh,
    '-NoProfile',
    '-ExecutionPolicy',
    'Bypass',
    '-File',
    files('scripts/meson-deploy-build.ps1'),
    windeployqt,
    meson.current_build_dir() / 'autoclicker.exe',
    meson.current_build_dir() / 'autoclicker.pdb',
    build_deploy_dir,
    meson.project_source_root() / 'qml',
    deploy_mode,
    '@OUTPUT@',
  ],
)

meson.add_install_script(
  pwsh,
  '-NoProfile',
  '-ExecutionPolicy',
  'Bypass',
  '-File',
  files('scripts/meson-install-windeployqt.ps1'),
  windeployqt,
  'bin/autoclicker.exe',
  meson.project_source_root() / 'qml',
  deploy_mode,
)
