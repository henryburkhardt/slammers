if (pixel - threshold > ptr_arr[3][col - 3]) {
    if (pixel - threshold > ptr_arr[6][col]) {
        if (pixel - threshold > ptr_arr[3][col + 3]) {
            if (pixel - threshold > ptr_arr[5][col - 2]) {
                if (pixel - threshold > ptr_arr[5][col + 2]) {
                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[6][col - 1]) {
                            if (pixel - threshold > ptr_arr[0][col]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                            else if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                        else {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                        else {
                                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel - threshold > ptr_arr[0][col]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[6][col + 1]) {
                        if (pixel - threshold > ptr_arr[0][col]) {
                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel - threshold > ptr_arr[0][col]) {
                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (pixel + threshold < ptr_arr[5][col + 2]) {
                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[0][col]) {
                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                if (pixel - threshold > ptr_arr[0][col]) {
                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (pixel + threshold < ptr_arr[5][col - 2]) {
                if (pixel - threshold > ptr_arr[0][col + 1]) {
                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                            if (pixel - threshold > ptr_arr[0][col]) {
                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {
                if (pixel - threshold > ptr_arr[0][col + 1]) {
                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                            if (pixel - threshold > ptr_arr[0][col]) {
                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (pixel + threshold < ptr_arr[3][col + 3]) {
            if (pixel - threshold > ptr_arr[1][col - 2]) {
                if (pixel - threshold > ptr_arr[5][col - 2]) {
                    if (pixel - threshold > ptr_arr[0][col]) {
                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[0][col]) {
                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[5][col + 2]) {
                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[6][col + 1]) {
                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[0][col - 1]) {
                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (pixel + threshold < ptr_arr[5][col - 2]) {
                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[2][col + 3]) {
                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[0][col]) {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[2][col + 3]) {
                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                if (pixel + threshold < ptr_arr[0][col]) {
                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (pixel + threshold < ptr_arr[1][col - 2]) {
                if (pixel - threshold > ptr_arr[5][col + 2]) {
                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                            if (pixel - threshold > ptr_arr[5][col - 2]) {
                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[2][col - 3]) {
                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                            if (pixel + threshold < ptr_arr[0][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (pixel + threshold < ptr_arr[5][col + 2]) {
                    if (pixel + threshold < ptr_arr[0][col]) {
                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    if (pixel + threshold < ptr_arr[2][col - 3]) {
                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                            if (pixel + threshold < ptr_arr[0][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {
                if (pixel - threshold > ptr_arr[4][col + 3]) {
                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (pixel + threshold < ptr_arr[4][col + 3]) {
                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                if (pixel + threshold < ptr_arr[0][col]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else {
            if (pixel - threshold > ptr_arr[2][col - 3]) {
                if (pixel - threshold > ptr_arr[4][col - 3]) {
                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                    }
                                    else if (pixel + threshold < ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                if (pixel - threshold > ptr_arr[0][col]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[0][col]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[5][col - 2]) {
                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[0][col - 1]) {
                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                            if (pixel - threshold > ptr_arr[5][col - 2]) {
                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else if (pixel + threshold < ptr_arr[6][col]) {
        if (pixel - threshold > ptr_arr[1][col + 2]) {
            if (pixel - threshold > ptr_arr[0][col]) {
                if (pixel - threshold > ptr_arr[3][col + 3]) {
                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[2][col + 3]) {
                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (pixel + threshold < ptr_arr[3][col + 3]) {
                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[5][col - 2]) {
                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[4][col - 3]) {
                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[2][col + 3]) {
                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (pixel + threshold < ptr_arr[0][col]) {
                if (pixel + threshold < ptr_arr[4][col - 3]) {
                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                            if (pixel + threshold < ptr_arr[3][col + 3]) {
                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {
                if (pixel + threshold < ptr_arr[4][col - 3]) {
                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                    if (pixel + threshold < ptr_arr[3][col + 3]) {
                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (pixel + threshold < ptr_arr[1][col + 2]) {
            if (pixel - threshold > ptr_arr[4][col + 3]) {
                if (pixel - threshold > ptr_arr[6][col - 1]) {
                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[0][col]) {
                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (pixel + threshold < ptr_arr[4][col + 3]) {
                if (pixel - threshold > ptr_arr[2][col + 3]) {
                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (pixel + threshold < ptr_arr[2][col + 3]) {
                    if (pixel - threshold > ptr_arr[0][col]) {
                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[6][col - 1]) {
                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[5][col - 2]) {
                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                    if (pixel + threshold < ptr_arr[3][col + 3]) {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[0][col]) {
                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                    if (pixel + threshold < ptr_arr[3][col + 3]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[5][col + 2]) {
                            if (pixel + threshold < ptr_arr[3][col + 3]) {
                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[6][col + 1]) {
                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                    if (pixel + threshold < ptr_arr[3][col + 3]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[3][col + 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel + threshold < ptr_arr[3][col + 3]) {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                    if (pixel - threshold > ptr_arr[0][col]) {
                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {
                if (pixel - threshold > ptr_arr[6][col - 1]) {
                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                            if (pixel - threshold > ptr_arr[5][col - 2]) {
                                if (pixel - threshold > ptr_arr[0][col]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else {
            if (pixel - threshold > ptr_arr[6][col - 1]) {
                if (pixel - threshold > ptr_arr[0][col + 1]) {
                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                            if (pixel - threshold > ptr_arr[0][col]) {
                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (pixel + threshold < ptr_arr[6][col - 1]) {
                if (pixel + threshold < ptr_arr[4][col - 3]) {
                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                            if (pixel + threshold < ptr_arr[3][col + 3]) {
                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else {
        if (pixel - threshold > ptr_arr[0][col]) {
            if (pixel - threshold > ptr_arr[0][col + 1]) {
                if (pixel - threshold > ptr_arr[0][col - 1]) {
                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                    }
                                    else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[3][col + 3]) {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[2][col + 3]) {
                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (pixel + threshold < ptr_arr[0][col]) {
            if (pixel + threshold < ptr_arr[4][col + 3]) {
                if (pixel + threshold < ptr_arr[0][col - 1]) {
                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                    if (pixel + threshold < ptr_arr[3][col + 3]) {
                                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[5][col + 2]) {
                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                            if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[6][col + 1]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[3][col + 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                        if (pixel + threshold < ptr_arr[3][col + 3]) {
                                            if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                if (pixel + threshold < ptr_arr[3][col + 3]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
else if (pixel + threshold < ptr_arr[3][col - 3]) {
    if (pixel - threshold > ptr_arr[0][col]) {
        if (pixel - threshold > ptr_arr[5][col + 2]) {
            if (pixel - threshold > ptr_arr[2][col + 3]) {
                if (pixel - threshold > ptr_arr[4][col + 3]) {
                    if (pixel - threshold > ptr_arr[6][col]) {
                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[0][col + 1]) {
                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[1][col + 2]) {
                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                    if (pixel - threshold > ptr_arr[3][col + 3]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                if (pixel - threshold > ptr_arr[3][col + 3]) {
                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[6][col]) {
                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[0][col - 1]) {
                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[6][col + 1]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel + threshold < ptr_arr[6][col]) {
                                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (pixel + threshold < ptr_arr[2][col + 3]) {
                if (pixel + threshold < ptr_arr[6][col + 1]) {
                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                            if (pixel + threshold < ptr_arr[6][col]) {
                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {
                if (pixel + threshold < ptr_arr[6][col + 1]) {
                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                    if (pixel + threshold < ptr_arr[6][col]) {
                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (pixel + threshold < ptr_arr[5][col + 2]) {
            if (pixel - threshold > ptr_arr[6][col]) {
                if (pixel - threshold > ptr_arr[2][col - 3]) {
                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (pixel + threshold < ptr_arr[6][col]) {
                if (pixel - threshold > ptr_arr[3][col + 3]) {
                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[4][col + 3]) {
                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[1][col - 2]) {
                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (pixel + threshold < ptr_arr[3][col + 3]) {
                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    if (pixel + threshold < ptr_arr[2][col - 3]) {
                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {
                if (pixel - threshold > ptr_arr[2][col - 3]) {
                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else {
            if (pixel - threshold > ptr_arr[2][col - 3]) {
                if (pixel - threshold > ptr_arr[4][col + 3]) {
                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (pixel + threshold < ptr_arr[2][col - 3]) {
                if (pixel + threshold < ptr_arr[0][col - 1]) {
                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                        if (pixel + threshold < ptr_arr[6][col]) {
                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else if (pixel + threshold < ptr_arr[0][col]) {
        if (pixel - threshold > ptr_arr[3][col + 3]) {
            if (pixel - threshold > ptr_arr[5][col - 2]) {
                if (pixel - threshold > ptr_arr[1][col + 2]) {
                    if (pixel - threshold > ptr_arr[6][col]) {
                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (pixel + threshold < ptr_arr[1][col + 2]) {
                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[6][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[4][col - 3]) {
                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                if (pixel + threshold < ptr_arr[2][col - 3]) {
                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[6][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (pixel + threshold < ptr_arr[5][col - 2]) {
                if (pixel - threshold > ptr_arr[2][col - 3]) {
                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                if (pixel - threshold > ptr_arr[6][col]) {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (pixel + threshold < ptr_arr[2][col - 3]) {
                    if (pixel - threshold > ptr_arr[6][col]) {
                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[6][col - 1]) {
                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[1][col + 2]) {
                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[6][col]) {
                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[1][col - 2]) {
                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[6][col - 1]) {
                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[6][col]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {
                if (pixel - threshold > ptr_arr[2][col + 3]) {
                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                if (pixel - threshold > ptr_arr[6][col]) {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (pixel + threshold < ptr_arr[2][col + 3]) {
                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                            if (pixel + threshold < ptr_arr[1][col + 2]) {
                                if (pixel + threshold < ptr_arr[2][col - 3]) {
                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (pixel + threshold < ptr_arr[3][col + 3]) {
            if (pixel - threshold > ptr_arr[1][col - 2]) {
                if (pixel + threshold < ptr_arr[6][col + 1]) {
                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                            if (pixel + threshold < ptr_arr[6][col]) {
                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (pixel + threshold < ptr_arr[1][col - 2]) {
                if (pixel + threshold < ptr_arr[1][col + 2]) {
                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                        if (pixel + threshold < ptr_arr[6][col]) {
                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                    if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[0][col - 1]) {
                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                            if (pixel + threshold < ptr_arr[6][col]) {
                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[0][col + 1]) {
                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                            if (pixel + threshold < ptr_arr[6][col]) {
                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[2][col + 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[6][col]) {
                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[6][col]) {
                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel + threshold < ptr_arr[6][col]) {
                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel + threshold < ptr_arr[6][col]) {
                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                if (pixel + threshold < ptr_arr[6][col]) {
                                    if (pixel + threshold < ptr_arr[2][col - 3]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {
                if (pixel + threshold < ptr_arr[6][col + 1]) {
                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                            if (pixel + threshold < ptr_arr[6][col]) {
                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else {
            if (pixel + threshold < ptr_arr[4][col - 3]) {
                if (pixel + threshold < ptr_arr[2][col - 3]) {
                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[6][col - 1]) {
                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel + threshold < ptr_arr[6][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[1][col - 2]) {
                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[6][col]) {
                                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[0][col - 1]) {
                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                if (pixel + threshold < ptr_arr[6][col]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[6][col]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel + threshold < ptr_arr[6][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else {
        if (pixel - threshold > ptr_arr[6][col]) {
            if (pixel - threshold > ptr_arr[2][col + 3]) {
                if (pixel - threshold > ptr_arr[6][col - 1]) {
                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                    if (pixel - threshold > ptr_arr[3][col + 3]) {
                                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (pixel + threshold < ptr_arr[6][col]) {
            if (pixel + threshold < ptr_arr[6][col + 1]) {
                if (pixel + threshold < ptr_arr[6][col - 1]) {
                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                    if (pixel + threshold < ptr_arr[2][col - 3]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[4][col + 3]) {
                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                if (pixel + threshold < ptr_arr[2][col - 3]) {
                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                            if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[4][col - 3]) {
                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[3][col + 3]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                if (pixel + threshold < ptr_arr[2][col - 3]) {
                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
else {
    if (pixel - threshold > ptr_arr[3][col + 3]) {
        if (pixel - threshold > ptr_arr[6][col]) {
            if (pixel - threshold > ptr_arr[2][col + 3]) {
                if (pixel - threshold > ptr_arr[4][col + 3]) {
                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                    }
                                    else if (pixel + threshold < ptr_arr[6][col + 1]) {
                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                if (pixel - threshold > ptr_arr[0][col]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[0][col]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[5][col + 2]) {
                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[0][col + 1]) {
                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[4][col - 3]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (pixel + threshold < ptr_arr[6][col]) {
            if (pixel - threshold > ptr_arr[0][col - 1]) {
                if (pixel - threshold > ptr_arr[4][col + 3]) {
                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                        if (pixel - threshold > ptr_arr[0][col]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                        if (pixel - threshold > ptr_arr[0][col]) {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[0][col]) {
                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[0][col]) {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                if (pixel - threshold > ptr_arr[0][col]) {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else {
            if (pixel - threshold > ptr_arr[0][col - 1]) {
                if (pixel - threshold > ptr_arr[4][col + 3]) {
                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                if (pixel - threshold > ptr_arr[0][col]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                    if (pixel - threshold > ptr_arr[0][col]) {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else if (pixel + threshold < ptr_arr[3][col + 3]) {
        if (pixel - threshold > ptr_arr[0][col]) {
            if (pixel + threshold < ptr_arr[6][col - 1]) {
                if (pixel + threshold < ptr_arr[2][col + 3]) {
                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                    if (pixel + threshold < ptr_arr[6][col]) {
                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[5][col - 2]) {
                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[6][col]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[6][col]) {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                if (pixel + threshold < ptr_arr[6][col]) {
                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (pixel + threshold < ptr_arr[0][col]) {
            if (pixel + threshold < ptr_arr[4][col + 3]) {
                if (pixel + threshold < ptr_arr[2][col + 3]) {
                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[6][col + 1]) {
                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[6][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[1][col + 2]) {
                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel + threshold < ptr_arr[6][col]) {
                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[0][col + 1]) {
                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                if (pixel + threshold < ptr_arr[6][col]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[6][col]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[2][col - 3]) {
                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[6][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                            if (pixel + threshold < ptr_arr[1][col + 2]) {
                                if (pixel + threshold < ptr_arr[2][col - 3]) {
                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else {
            if (pixel + threshold < ptr_arr[6][col - 1]) {
                if (pixel + threshold < ptr_arr[2][col + 3]) {
                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                if (pixel + threshold < ptr_arr[6][col]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                    if (pixel + threshold < ptr_arr[6][col]) {
                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
