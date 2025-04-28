import unittest
from run_script_helper import run_script

class TestTableFull(unittest.TestCase):
    def test_prints_error_message_when_table_is_full(self):
        script = [f"insert {i} user{i} person{i}@example.com;" for i in range(1, 1402)]
        script.append(".exit;")
        result = run_script(script)
        self.assertIn("sqlcs50> Error: Table full.", result)
