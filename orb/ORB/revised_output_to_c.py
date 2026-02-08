import io
import sys


class TreeNode:
    """
    Represents a node in the decision tree that has three corresponding children:
    The first child is the trees decision if the pixels are similar in color.
    The second child is the trees decision if the comparison pixel was darker than the center pixel.
    The third child is the trees decision if the comparison pixel was lighter than the center pixel.
    The value variable holds what the trees next decision negative values represent final decisions while positive values are next index.
    """
    def __init__(self):
        self.children = []
        self.value = -3

    def __eq__(self, value):
        return self.value == value.value and self.children == value.children

    def generate_tree_text(self, indent_level=''):
        if self.value < 0:
            if self.value == -1:
                print('error')
            else:
                print(indent_level + 'nmsVals[row * width + col] = get_corner_score(img, row, col);')
            return
        one_two_equal = self.children[0] == self.children[1]
        two_three_equal = self.children[1] == self.children[2]
        one_three_equal = self.children[0] == self.children[2]

        print_else = False
        
        if two_three_equal:
            if self.children[1].value != -1:
                print(indent_level + f'if (pixel + threshold < surrPoints[{self.value}] || pixel - threshold > surrPoints[{self.value}]) {{')
                self.children[1].generate_tree_text(indent_level + '    ')
                print(indent_level + '}')
                print_else = True
        else:
            if not one_two_equal:
                if self.children[1].value != -1:
                    print(indent_level + f'if (pixel - threshold > surrPoints[{self.value}]) {{')
                    self.children[1].generate_tree_text(indent_level + '    ')
                    print(indent_level + '}')
                    print_else = True
            if not one_three_equal:
                if self.children[2].value != -1:
                    print(indent_level + ('else ' if print_else else '') + f'if (pixel + threshold < surrPoints[{self.value}]) {{')
                    self.children[2].generate_tree_text(indent_level + '    ')
                    print(indent_level + '}')
                    print_else = True
        if self.children[0].value != -1:
            if self.children[1].value == -1 or self.children[2].value == -1:
                print(indent_level + ('else ' if print_else else '') + f'if (pixel + threshold >= surrPoints[{self.value}] && pixel - threshold <= surrPoints[{self.value}]) {{')
                self.children[0].generate_tree_text(indent_level + '    ')
                print(indent_level + '}')
            else:
                if print_else:
                    print(indent_level + 'else {')
                self.children[0].generate_tree_text(indent_level=indent_level + ('    ' if print_else else ''))
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
        # adapt depth to correct value
        while not line.startswith(' ' * depth):
            depth -= 1
            stack.pop()
        # if the stack exists, make a new node for this line
        if stack:
            stack[-1].children.append(TreeNode())
            stack.append(stack[-1].children[-1])
        # start the stack
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