import unittest
from run_script_helper import run_script, clean_output

class TestSelectMultiLevel(unittest.TestCase):
    def test_prints_all_rows_in_multi_level_tree(self):
        script = [f"insert {i} user{i} person{i}@example.com;" for i in range(1, 35)]
        script.append("select;")
        script.append(".exit;")
        result = run_script(script)
        cleaned = clean_output(result)

        for i in range(1, 35):
            self.assertIn(f"({i}, user{i}, person{i}@example.com)", cleaned)
        self.assertIn("Executed.", cleaned)
