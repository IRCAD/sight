#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os

def main():
    count = 0
    added_root = []

    for root, dirs, files in os.walk("."):

        if root.startswith("./build"):
            continue

        if root.startswith("./.git"):
            continue

        if root.startswith("./.vscode"):
            continue

        if root.startswith("./cmake"):
            continue

        if root.startswith("./.idea"):
            continue

        if root.startswith("./.idea"):
            continue

        if root != ".":
            must_break = False
            for root_test in added_root:
                if root.startswith(root_test):
                    must_break = True
                    break

            if must_break:
                continue

        if root != ".":
            if "CMakeLists.txt" in files:
                added_root.append(root)
            else:
                count = count + 1
                print(root + " " + str(count))

                cmakelist_path = os.path.join(root, "CMakeLists.txt")
                with open(cmakelist_path, 'w') as cmakelist_file:
                    for subdir in dirs:
                        cmakelist_file.write(f"add_subdirectory({subdir})\n")

                # added_root.append(root)


if __name__ == "__main__":
    main()
