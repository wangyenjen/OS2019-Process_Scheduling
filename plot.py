import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np

SCHD_TYPES = ['FIFO', 'RR', 'SJF', 'PSJF']
TOTAL_COUNT = 5
W_FACTOR = 10
H_FACTOR = 10
H_OFFSET = 10
H_DIST = 20
GREENS = cm.get_cmap('Greens', 8)
REDS = cm.get_cmap('Reds', 8)

for schd in SCHD_TYPES:
    for n in range(1, TOTAL_COUNT+1):

        fig, ax = plt.subplots()

        test_file = 'test/' + schd + '_' + str(n) + '.txt'
        schd_file = 'outputs/schd_' + schd + '_' + str(n) + '.txt'
        calc_file = 'outputs/calc_' + schd + '_' + str(n) + '.txt'

        y_ticklabels = []
        name_dict, indx_dict = dict(), dict()
        test_lines = open(test_file, 'r').read().splitlines()
        for i in range(2, len(test_lines)):
            name = test_lines[i].split()[0]
            y_ticklabels.append(name)
            name_dict[name] = i-2
            indx_dict[i-2] = name

        num = len(y_ticklabels)

        schd_times = list(range(num))
        schd_lines = open(schd_file, 'r').read().splitlines()
        for l in schd_lines:
            if l[:10] == '[Project1]':
                _, indx, st, ed = l.split()
                st = float(st)
                ed = float(ed)
                # name = indx_dict[indx]
                schd_times[int(indx)] = (st, ed)

        calc_times = list(range(num))
        calc_lines = open(calc_file, 'r').read().splitlines()
        for l in calc_lines:
            if l[:9] == 'error of ':
                _, _, name, _, _, err_st, _, _, err_ed = l.split()
                err_st = float(err_st[:-1])
                err_ed = float(err_ed)
                indx = name_dict[name[:-1]]
                calc_times[indx] = (schd_times[indx][0] + err_st,
                                    schd_times[indx][1] + err_ed)
            elif l[:14] == 'total error = ':
                total_err = float(l.split()[-1])

        min_time, max_time = 1e100, 0

        for i in range(num):
            min_time = min(min_time, schd_times[i][0], calc_times[i][0])
            max_time = max(max_time, schd_times[i][0], calc_times[i][0])

        for i in range(num):
            schd_times[i] = (schd_times[i][0] - min_time, schd_times[i][1] - min_time)
            calc_times[i] = (calc_times[i][0] - min_time, calc_times[i][1] - min_time)
        #     schd_times[i] = (schd_times[i][0] * W_FACTOR / max_time,
        #                      schd_times[i][1] * W_FACTOR / max_time)
        #     calc_times[i] = (calc_times[i][0] * W_FACTOR / max_time,
        #                      calc_times[i][1] * W_FACTOR / max_time)

        x_ticks, y_ticks = [], []
        for i in range(num):
            ax.broken_barh([(schd_times[i][0], schd_times[i][1] - schd_times[i][0])],
                           (H_OFFSET + 2*i*H_FACTOR, H_FACTOR), facecolors=GREENS((i+1)/num))
            y_ticks.append(H_OFFSET + 2*i*H_FACTOR + H_FACTOR / 2)
            ax.broken_barh([(calc_times[i][0], calc_times[i][1] - calc_times[i][0])],
                           ((2*num-1+2*i) * H_FACTOR + H_OFFSET + H_DIST, H_FACTOR),
                           facecolors=REDS((i+1)/num))
            x_ticks.append(calc_times[i][0])
            x_ticks.append(calc_times[i][1])
            y_ticks.append((2*num-1+2*i) * H_FACTOR + H_OFFSET + H_DIST + H_FACTOR/ 2)

        x_ticks = np.unique(sorted(x_ticks))
        y_ticks = sorted(y_ticks)
        x_ticklabels = ['{:.1f}'.format(x_tick) for x_tick in x_ticks]

        ax.set_title(schd + '_' + str(n) + ' (total error = {} secs)'.format(total_err))
        ax.set_xlabel('secs since start')
        ax.xaxis.set_label_coords(0.5, -0.12)
        ax.set_ylabel('process names (our results v.s. ground truth)')

        ax.set_xticks(x_ticks)
        x_labels = ax.set_xticklabels(x_ticklabels, rotation=0)
        for i, x_label in enumerate(x_labels):
            x_label.set_y(x_label.get_position()[1] - (i % 3) * 0.032)
        ax.set_yticks(y_ticks)
        ax.set_yticklabels(y_ticklabels * 2)
        ax.grid(True)

        plt.tight_layout()
        # plt.show()
        plt.savefig('imgs/' + schd + '_' + str(n) + '.png')
        # exit()
