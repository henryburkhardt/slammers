import io

class TreeNode:
    def __init__(self):
        self.children = []
        self.value = -3


def parse_line(line):
    stripped_line = line.strip()
    if line == 'not corner':
        return -1
    if line == 'corner':
        return -2
    return int(stripped_line)


def load_tree(output:io.TextIOWrapper):
    tree = [line.rstrip('\n') for line in output.readlines()]
    depth = 0
    stack = []
    for line in tree:
        if line.startswith(' ' * (depth + 1)):
            depth += 1
            stack[-1].children.append(TreeNode())
            stack.append(stack[-1].children[-1])
        while not line.startswith(' ' * depth + 1)
        value = parse_line(line)
        stack[-1] = value

def main():
    with open('output.txt') as output:
        load_tree(output)



if __name__ == "__main__":
    main()