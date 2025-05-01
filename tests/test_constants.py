import os
import unittest
from run_script import run_script, clean_output

test_name = os.path.basename(__file__)
class TestConstants(unittest.TestCase):
    def test_prints_constants(self):
        script = [
            ".constants;",
            ".exit;",
        ]
        result = run_script(script, test_name, "test_prints_constants")
        self.assertEquals(["sqlcs50> Constants:",
                      "ROW_SIZE: 293",
                      "COMMON_NODE_HEADER_SIZE: 6",
                      "LEAF_NODE_HEADER_SIZE: 14",
                      "LEAF_NODE_CELL_SIZE: 297",
                      "LEAF_NODE_SPACE_FOR_CELLS: 4082",
                      "LEAF_NODE_MAX_CELLS: 13",
                      "INTERNAL_NODE_CELL_SIZE: 8",
                      "sqlcs50>"], result)
