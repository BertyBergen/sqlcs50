import unittest
import os
from run_script import run_script, clean_output

test_name = os.path.basename(__file__)[:-3]

class TestSelectMultiLevel(unittest.TestCase):
    def test_rows_multi_level_tree(self):
        script = [f"insert {i} user{i} person{i}@example.com;" for i in range(1, 15)]
        script.append("select;")
        script.append(".exit")
        result = run_script(script, test_name, "test_rows_multi_level_tree")
        cleaned = clean_output(result)

        for i in range(1, 15):
            self.assertIn(f"({i}, user{i}, person{i}@example.com)", cleaned)
        self.assertIn("Executed.", cleaned)
                        