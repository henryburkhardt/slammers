import io
import sys

CIRCLE_POINTS = [
    [0, -3],
    [1, -3],
    [2, -2],
    [3, -1],
    [3, 0],
    [3, 1],
    [2, 2],
    [1, 3],
    [0, 3],
    [-1, 3],
    [-2, 2],
    [-3, 1],
    [-3, 0],
    [-3, -1],
    [-2, -2],
    [-1, -3]
];

class TreeNode:
    def __init__(self):
        self.children = []
        self.value = -3

    def __eq__(self, value):
        return self.value == value.value and self.children == value.children

    def generate_tree_text(self, indent_level='', first_call = True):
        if first_call:
            print('int comp_pixel, pixel;')
            print('comp_pixel = raw_data[row * width + col];')
        if self.value < 0:
            if self.value == -1:
                print('error')
            else:
                print(indent_level + 'differences[row][col] = calculateDifference(img, row, col); std::cout << "value stored" << std::endl;')
            return
        one_two_equal = self.children[0] == self.children[1]
        two_three_equal = self.children[1] == self.children[2]
        one_three_equal = self.children[0] == self.children[2]

        print_else = False

        y = CIRCLE_POINTS[self.value][1]
        x = CIRCLE_POINTS[self.value][0]
        print(indent_level + f'pixel = raw_data[(row{((' + ' if y >= 0 else ' - ') + str(abs(y))) if y != 0 else ''}) * width + col{((' + ' if x >= 0 else ' - ') + str(abs(x))) if x != 0 else ''}];')
        
        # TODO stop code from entering else statement when self.children.value == -1
        if two_three_equal:
            if self.children[1].value != -1:
                print(indent_level + 'if (pixel + threshold < comp_pixel || pixel - threshold > comp_pixel) {')
                self.children[1].generate_tree_text(indent_level + '    ', first_call=False)
                print(indent_level + '}')
                print_else = True
        elif not one_two_equal or not one_three_equal:
            if not one_two_equal:
                if self.children[1].value != -1:
                    print(indent_level + 'if (pixel + threshold < comp_pixel) {')
                    self.children[1].generate_tree_text(indent_level + '    ', first_call=False)
                    print(indent_level + '}')
                    print_else = True
            if not one_three_equal:
                if self.children[2].value != -1:
                    print(indent_level + ('else ' if print_else else '') + 'if (pixel - threshold > comp_pixel) {')
                    self.children[2].generate_tree_text(indent_level + '    ', first_call=False)
                    print(indent_level + '}')
                    print_else = True
        if self.children[0].value != -1:
            if self.children[1].value == -1 or self.children[2].value == -1:
                print(indent_level + ('else ' if print_else else '') + 'if (pixel + threshold >= comp_pixel && pixel - threshold <= comp_pixel) {')
                self.children[0].generate_tree_text(indent_level + '    ', first_call=False)
                print(indent_level + '}')
            else:
                if print_else:
                    print(indent_level + 'else {')
                self.children[0].generate_tree_text(indent_level=indent_level + ('    ' if print_else else ''), first_call=False)
                if print_else:
                    print(indent_level + '}')


def parse_line(line):
    stripped_line = line.strip(' ')
    if stripped_line == 'not corner':
        return -1
    if stripped_line == 'corner':
        return -2
    return int(stripped_line)


def load_tree(output:io.TextIOWrapper):
    tree = [line.rstrip('\n') for line in output.readlines()]
    depth = 0
    stack = []
    for line in tree:
        value = parse_line(line)
        while not line.startswith(' ' * depth):
            depth -= 1
            stack.pop()
        if stack:
            stack[-1].children.append(TreeNode())
            stack.append(stack[-1].children[-1])
        else:
            stack.append(TreeNode())
        stack[-1].value = value
        depth += 1
    return stack[0]

def main():
    tree = None
    input_file = 'input.txt' if len(sys.argv) == 1 else sys.argv[1]
    with open(input_file) as input:
        tree = load_tree(input)
    tree.generate_tree_text()



if __name__ == "__main__":
    main()