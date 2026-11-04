import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from sr import config, makefile


class SourceIndexTests(unittest.TestCase):
    def test_cross_folder_headers_and_staging(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            src = root / 'src'
            for folder in ('SYS', 'GAME'):
                (src / folder).mkdir(parents=True)
            (src / 'SYS' / 'PC.INC').write_bytes(b'INCLUDE VIDEO.INC\r\n')
            (src / 'GAME' / 'VIDEO.INC').write_bytes(b'INCLUDE PC.INC\r\n')
            asm = src / 'GAME' / 'DRAW.ASM'
            asm.write_bytes(b'INCLUDE PC.INC\r\n')
            with patch.object(config, 'SRC', src):
                self.assertEqual(config.source_path('pc.inc'), src / 'SYS' / 'PC.INC')
                self.assertEqual(makefile.header_deps(asm), ['SYS\\PC.INC', 'GAME\\VIDEO.INC'])
                config.stage_headers(root / 'flat', flat=True)
                config.stage_headers(root / 'tree')
            for folder, name in (('SYS', 'PC.INC'), ('GAME', 'VIDEO.INC')):
                data = (src / folder / name).read_bytes()
                self.assertEqual((root / 'flat' / name).read_bytes(), data)
                self.assertEqual((root / 'tree' / folder / name).read_bytes(), data)
            self.assertFalse((root / 'flat' / 'DRAW.ASM').exists())

    def test_duplicate_names_are_rejected_ignoring_case(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'A').mkdir()
            (root / 'B').mkdir()
            (root / 'A' / 'PC.H').touch()
            (root / 'B' / 'pc.h').touch()
            with patch.object(config, 'SRC', root), self.assertRaisesRegex(ValueError, 'duplicate source name'):
                config.source_index()

    def test_native_include_paths_cover_every_project_header(self):
        env = dict(item.split('=', 1) for item in config.dos_environment())
        folders = env['INCLUDE'].split(';')
        self.assertTrue({p.parent.name for p in config.source_files('.H', '.INC')} <= set(folders))
        self.assertLess(max(map(len, (config.SRC / 'SRENV.BAT').read_text().splitlines())), 126)
