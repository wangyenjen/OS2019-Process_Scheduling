# 2019 Operating System Project 1 - Process Scheduling

## Member

* 資工二 b06902093 王彥仁
    * 寫kernel code、編譯kernel
    * RR、SJF、PSJF實作
    * 比較與分析效能
* 資工二 b06902026 吳秉柔
    * 提供PSJF的原理與實作方式
    * 設計執行框架
    * FIFO實作
    * 程式碼整理與排版
* 資工二 b06902041 吳采耘
    * 提供RR的原理與實作方式
* 資工二 b06902054 蔡宥杏
    * 提供SJF的原理與實作方式
* 醫學三 b05401009 謝德威
    * 提供FIFO的原理與實作方式
* 電機五 b03901056 孫凡耕
    * 檢查各實作細節
    * 計算理論最優解與誤差
    * 比較與分析效能

## Design

### First-Come, First-Served Scheduling

* 原理
    * 顧名思義，對於每個process，先來的先執行。
* 執行方式
    * 將所有process按照ready time由小到大排序。
    * 對於每個process，若其ready time為當前的時間，則將該process放入一個queue中。
    * 如果目前沒有在執行process，並且queue中有process，則從queue中取出process並執行，直到該process執行結束。
    * 執行中的process會被賦予一個較高的priotity。
```
change_priority() {
    if (no process is running) and (there exist runnable processes):
        run the next process;
    else if (some process is running) and (there exist other runnable proccess):
        increase the priority of the next process;
}

SIGCHLD_signal_handler() {
    wait for completed child process;
    number of finished processes += 1;
    pop one process from queue;
    if (number of finished processes == number of processes):
        exit();
}

FIFO() {
    sort(comingProcess);
    change SIGCHLD from block to receive;
    while unfinished tasks exist:
        change_priority();	
        for all (processes that are ready at this moment t):
            create process (fork and execute);
            push the next ready process into queue;
            change_priority();
        run a unit of time;
}
```
* 參考資料
    * Operating System Concepts Ch.3, Ch.5



### Round-Robin Scheduling

* 原理
    * 利用計時器固定週期，讓正在執行的process在執行一個週期後進入queue中，並從queue中選定下一個執行的process。
* 執行方式
    * 將所有process按照ready time由小到大排序。
    * 於每個process，若其ready time為當前的時間，則將該process放入一個queue中。
    * 如果目前沒有在執行process，並且queue中有process，則從queue中取出process執行，並計算該process是否會在此執行周期內執行結束。如果沒有結束，在執行一個周期後將remaining execution time減少一個週期，並將該process放入queue中。
    * 執行中的process會被賦予一個較高的priotity。

```
change_priority() {
    if (no process is running) and (there exist runnable processes):
        run the next process;
    else if (some process is running) and (there exist other runnable proccess):
        increase the priority of the next process;
}

SIGCHLD_signal_handler() {
    wait for completed child process;
    number of finished processes += 1;
    pop one process from queue;
    run = FALSE;
    counting_time = 0;
    if (number of finished processes == number of processes):
        exit();
}

RR() {
    sort(comingProcess);
    change SIGCHLD from block to receive;
    while unfinished tasks exist:
        change_priority();	
        for all (processes that are ready at this moment t):
	    create process (fork and execute);
	    push the next ready process into queue;
	    change_priority();
        run a unit of time;
	counting_time++;
	if (run == TRUE and counting_time == time_quantum):
	    pop one process from queue;
	    expected remaining time of process -= time_quantum;
	    set the priority of the process to LOW;
	    push the process back to queue;
	    run = FALSE;
	    change_priority();
}

```
* 參考資料
    * [05. 行程排班/排程 (Process Scheduling)](https://sls.weco.net/node/21325)
    * [Round-robin排程(作業系統)](https://tw.answers.yahoo.com/question/index?qid=20110602000010KK06795)



### Shortest Job First (SJF)
 
* 原理概念
    * 選擇執行時間最短的process來執行，於實際應用的困難點在於如何估算process所需的執行時間。此處應用的SJF是non-preemptive的，不中斷仍在執行的task，另外有PSJF。
* 執行方式 
    * 將所有process照ready time由小到大排序，若ready time相同，再按照execution time由小到大排序。
    * 對於每個process，若其ready time為當前的時間，則將該process放入一個heap中，此heap是照execution time由小到大排序的。
    * 如果目前沒有在執行process，並且heap中有process，則取出execution time最小的process並執行，直到該process執行結束。
    * 執行中的process會被賦予一個較高的priotity。
```
change_priority() {
    if (no process is running) and (there exist runnable processes):
        run the next process;
    else if (some process is running) and (there exist other runnable proccess):
        increase the priority of the next process;
}

SIGCHLD_signal_handler() {
    wait for completed child process;
    number of finished processes += 1;
    run = FALSE;
    if (number of finished processes == number of processes):
        exit();
}

SJF() {
    sort(comingProcess) by execute time;
    sort(comimgProcess) by ready time;
    change SIGCHLD from block to receive;
    while unfinished tasks exist:
        change_priority();	
        for all (processes that are ready at this moment t):
	    decrease the priority of the next process;
	    create process (fork and execute);
	    insert the next ready process into heap;
	    change_priority();
        run a unit of time;
}

```
* 參考
    * [行程排班/排程 (Process Scheduling)](https://sls.weco.net/node/21325)
    


### Preemptive Shortest Job First (PSJF)

* 原理概念
    * 每次選擇剩餘執行時間最短的 process 來執行，當新產生的 process 執行時間更短時，就可以插隊。
* 執行方式 
    * 將所有process照ready time由小到大排序，若ready time相同，再按照execution time由小到大排序。
    * 對於每個process，若其ready time為當前的時間，則將該process放入一個heap中，此heap是照remaining execution time由小到大排序的。
    * 若當前heap頂端process的remaining execution time比目前執行中的process更小，則將當前執行的process換成該從heap取出的process，並將原本執行的process放入heap中。
    * 如果目前沒有執行任何process，並且heap中有process，則取出execution time最小的process執行。
    * 執行中的process會被賦予一個較高的priotity。
```
change_priority() {
    if (no process is running) and (there exist runnable processes):
        run the next process;
    else if (some process is running) and (there exist other runnable proccess):
        increase the priority of the next process;
}

SIGCHLD_signal_handler() {
    wait for completed child process;
    number of finished processes += 1;
    run = FALSE;
    if (number of finished processes == number of processes):
        exit();
}

PSJF() {
    sort(comingProcess) by execute time;
    sort(comimgProcess) by ready time;
    change SIGCHLD from block to receive;
    while unfinished tasks exist:
        change_priority();	
        for all (processes that are ready at this moment t):
	    //block SIGCHLD in the functions to avoid race conditions
	    decrease the priority of the next 2 process;
	    create process (fork and execute);
	    insert the next ready process into heap;
	    change_priority();
    run a unit of time;
    check if any child process died when SIGCHLD was blocked;
}
```
## Test & Analysis

### FIFO
#### 測試資料1
資料內容：
```
FIFO
5
P1 0 500
P2 0 500
P3 0 500
P4 0 500
P5 0 500
```
執行結果：
```
P1 1030 
P2 1031
P3 1032
P4 1033
P5 1034
[Project1] 1030 1556267696.005955744 1556267697.447623058
[Project1] 1031 1556267697.448563280 1556267698.887764870
[Project1] 1032 1556267698.888653958 1556267700.375917190
[Project1] 1033 1556267700.377058738 1556267701.881777997
[Project1] 1034 1556267701.882504746 1556267703.341983999
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/hZF108V.png)

#### 測試資料2
資料內容：
```
FIFO
4
P1 0 80000
P2 100 5000
P3 200 1000
P4 300 1000
```
執行結果：
```
P1 3169
P2 3170
P3 3174
P4 3175
[Project1] 3169 1556267703.356366380 1556267937.327534262
[Project1] 3170 1556267937.328329747 1556267951.732974392
[Project1] 3174 1556267951.733685676 1556267954.660797710
[Project1] 3175 1556267954.662715126 1556267957.544203414
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/qt3epoS.png)


#### 測試資料3
資料內容：
```
FIFO
7
P1 0 8000
P2 200 5000
P3 300 3000
P4 400 1000
P5 500 1000
P6 500 1000
P7 600 4000
```
執行結果：
```
P1 4114
P2 4118
P3 4119
P4 4120
P5 4124
P6 4125
P7 4126
[Project1] 4114 1556267957.557357029 1556267981.033809368
[Project1] 4118 1556267981.034554538 1556267995.795331824
[Project1] 4119 1556267995.796093011 1556268004.546408140
[Project1] 4120 1556268004.547211199 1556268007.492391289
[Project1] 4124 1556268007.493287372 1556268010.434410310
[Project1] 4125 1556268010.435314787 1556268013.401105627
[Project1] 4126 1556268013.401778201 1556268025.149761910
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/Oo81ham.png)


#### 測試資料4
資料內容：
```
FIFO
4
P1 0 2000
P2 500 500
P3 500 200
P4 1500 500
```
執行結果：
```
P1 4448
P2 4452
P3 4453
P4 4463
[Project1] 4448 1556281099.394525838 1556281105.172335335
[Project1] 4452 1556281105.173256408 1556281106.611718370
[Project1] 4453 1556281106.612488762 1556281107.192480105
[Project1] 4463 1556281107.193478191 1556281108.657068430
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/CUXJqJw.png)


#### 測試資料5
資料內容：
```
FIFO
7
P1 0 8000
P2 200 5000
P3 200 3000
P4 400 1000
P5 400 1000
P6 600 1000
P7 600 4000
```
執行結果：
```
P1 4762
P2 4763
P3 4764
P4 4768
P5 4769
P6 4770
P7 4771
[Project1] 4762 1556268034.513253253 1556268057.593677271
[Project1] 4763 1556268057.594464663 1556268072.202658653
[Project1] 4764 1556268072.203379298 1556268081.023290764
[Project1] 4768 1556268081.023870239 1556268083.954713381
[Project1] 4769 1556268083.955627959 1556268086.854611338
[Project1] 4770 1556268086.855351215 1556268089.777458667
[Project1] 4771 1556268089.778370442 1556268101.602077366
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/35ooqqZ.png)


### RR
#### 測試資料1
資料內容：
```
RR
5
P1 0 500
P2 0 500
P3 0 500
P4 0 500
P5 0 500
```
執行結果：
```
P1 5371
P2 5372
P3 5373
P4 5374
P5 5375
[Project1] 5371 1556267012.172391630 1556267013.636782768
[Project1] 5372 1556267013.637596962 1556267015.144664635
[Project1] 5373 1556267015.145695418 1556267016.630127328
[Project1] 5374 1556267016.631057233 1556267018.118905020
[Project1] 5375 1556267018.119639198 1556267019.557252571
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/qhG5AeJ.png)


#### 測試資料2
資料內容：
```
RR
2
P1 600 4000
P2 800 5000
```
執行結果：
```
P1 5565
P2 5569
[Project1] 5565 1556267021.356233936 1556267043.244070124
[Project1] 5569 1556267022.789869600 1556267047.521132718
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/KHhCjlf.png)


#### 測試資料3
資料內容：
```
RR
6
P1 1200 5000
P2 2400 4000
P3 3600 3000
P4 4800 7000
P5 5200 6000
P6 5800 5000
```
執行結果：
```
P1 5693
P2 5706
P3 5718
P4 5731
P5 5735
P6 5742
[Project1] 5718 1556267059.666615102 1556267100.587747789
[Project1] 5693 1556267051.026896095 1556267103.548607283
[Project1] 5706 1556267055.328398416 1556267106.488141589
[Project1] 5742 1556267071.301305316 1556267129.855954499
[Project1] 5735 1556267068.404326454 1556267135.761305332
[Project1] 5731 1556267065.451713173 1556267138.594189703
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/H11TA0H.png)


#### 測試資料4
資料內容：
```
RR
7
P1 0 8000
P2 200 5000
P3 300 3000
P4 400 1000
P5 500 1000
P6 500 1000
P7 600 4000
```
執行結果：
```
P1 6712
P2 6713
P3 6717
P4 6718
P5 6719
P6 6720
P7 6724
[Project1] 6718 1556267143.030088707 1556267154.775837897
[Project1] 6719 1556267145.963340497 1556267157.761930919
[Project1] 6720 1556267147.398581715 1556267159.170775836
[Project1] 6717 1556267141.533656823 1556267181.354278683
[Project1] 6724 1556267148.892808209 1556267193.038339364
[Project1] 6713 1556267140.046027846 1556267197.514231450
[Project1] 6712 1556267138.621823815 1556267206.420814624
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/GgiodyK.png)


#### 測試資料5
資料內容：
```
RR
7
P1 0 8000
P2 200 5000
P3 200 3000
P4 400 1000
P5 400 1000
P6 600 1000
P7 600 4000
```
執行結果：
```
P1 8222
P2 8226
P3 8227
P4 8228
P5 8229
P6 8233
P7 8234
[Project1] 8228 1556267210.776174471 1556267222.339916321
[Project1] 8229 1556267212.202524675 1556267223.846046997
[Project1] 8233 1556267215.052176172 1556267226.813624358
[Project1] 8227 1556267209.326543274 1556267248.933188703
[Project1] 8234 1556267216.491265951 1556267260.721219795
[Project1] 8226 1556267207.897662196 1556267264.928738340
[Project1] 8222 1556267206.440100595 1556267273.542002480
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/odIDdsY.png)


### SJF
#### 測試資料1
資料內容：
```
SJF
4
P1 0 7000
P2 0 2000
P3 100 1000
P4 200 4000
```
執行結果：
```
P2 9036
P1 9037
P3 9041
P4 9042
[Project1] 9036 1556267273.563992439 1556267279.345748769
[Project1] 9041 1556267279.346689060 1556267282.255005355
[Project1] 9042 1556267282.255746710 1556267293.985676622
[Project1] 9037 1556267293.986441521 1556267314.813214073
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/JPFLGbb.png)


#### 測試資料2
資料內容：
```
SJF
5
P1 100 100
P2 100 4000
P3 200 200
P4 200 4000
P5 200 7000
```
執行結果：
```
P1 9449
P3 9450
P2 9454
P4 9455
P5 9456
[Project1] 9449 1556273082.261394371 1556273082.578534405
[Project1] 9450 1556273082.579450508 1556273083.175679321
[Project1] 9454 1556273083.176759269 1556273095.037849678
[Project1] 9455 1556273095.038662587 1556273106.628237103
[Project1] 9456 1556273106.629285202 1556273126.852447988
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/ClIj8xl.png)


#### 測試資料3
資料內容：
```
SJF
8
P1 100 3000
P2 100 5000
P3 100 7000
P4 200 10
P5 200 10
P6 300 4000
P7 400 4000
P8 500 9000
```
執行結果：
```
P1 11501
P4 11504
P5 11505
P6 11506
P7 11507
P2 11502
P3 11503
P8 11511
[Project1] 11501 1556267359.672181839 1556267368.468376440
[Project1] 11504 1556267368.469405546 1556267368.499879461
[Project1] 11505 1556267368.500644831 1556267368.531903565 
[Project1] 11506 1556267368.532659705 1556267380.221958006 
[Project1] 11507 1556267380.222597346 1556267392.072762336
[Project1] 11502 1556267392.073620507 1556267406.700528466
[Project1] 11503 1556267406.701579939 1556267427.329905502
[Project1] 11511 1556267427.330569346 1556267453.395551434  
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/Z8yg8LM.png)


#### 測試資料4
資料內容：
```
SJF
5
P1 0 3000
P2 1000 1000
P3 2000 4000
P4 5000 2000
P5 7000 1000
```
執行結果：
```
P1 16501
P2 16511
P3 16521
P5 16565
P4 16549
[Project1] 16501 1556267453.423024637 1556267462.192598479
[Project1] 16511 1556267462.194584693 1556267465.098985788
[Project1] 16521 1556267465.099863826 1556267476.652149328
[Project1] 16565 1556267476.653164431 1556267479.553047282
[Project1] 16549 1556267479.553812300 1556267485.353083034
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/rQiHiwY.png)


#### 測試資料5
資料內容：
```
SJF
4
P1 0 2000
P2 500 500
P3 1000 500
P4 1500 500
```
執行結果：
```
P1 16893
P2 16900
P3 16904
P4 16911
[Project1] 16893 1556267485.363140018 1556267491.273938096
[Project1] 16900 1556267491.274883902 1556267492.713872845
[Project1] 16904 1556267492.714582136 1556267494.157116128
[Project1] 16911 1556267494.157999645 1556267495.612737337
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/Qbq4kTy.png)


### PSJF
#### 測試資料1
資料內容：
```
PSJF
4
P1 0 10000
P2 1000 7000
P3 2000 5000
P4 3000 3000
```
執行結果：
```
P1 17182
P2 17192
P3 17202
P4 17212
[Project1] 17212 1556267504.357288589 1556267513.077936866
[Project1] 17202 1556267501.409855706 1556267524.696781902
[Project1] 17192 1556267498.485999811 1556267542.101250387
[Project1] 17182 1556267495.622864163 1556267568.145374822
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/LGtb5ss.png)


#### 測試資料2
資料內容：
```
PSJF
5
P1 0 3000
P2 1000 1000
P3 2000 4000
P4 5000 2000
P5 7000 1000
```
執行結果：
```
P1 17480
P2 17487
P3 17497
P4 17527
P5 17546
[Project1] 17487 1556267571.054433036 1556267574.009092155
[Project1] 17480 1556267568.180510843 1556267579.910755765
[Project1] 17527 1556267582.748044684 1556267588.574176808
[Project1] 17546 1556267588.637914347 1556267591.562909837
[Project1] 17497 1556267579.911440537 1556267600.309033345
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/106kAEm.png)


#### 測試資料3
資料內容：
```
PSJF
4
P1 0 2000
P2 500 500
P3 1000 500
P4 1500 500
```
執行結果：
```
P1 17625
P2 17629
P3 17636
P4 17640
[Project1] 17629 1556267601.726423729 1556267603.201099864
[Project1] 17636 1556267603.201947820 1556267604.618702682
[Project1] 17640 1556267604.619486251 1556267606.076288554
[Project1] 17625 1556267600.322827757 1556267610.394578261
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/HLxSr6o.png)


#### 測試資料4
資料內容：
```
PSJF
4
P1 0 7000
P2 0 2000
P3 100 1000
P4 200 4000
```
執行結果：
```
P2 17740
P3 17742
P4 17743
P1 17741
[Project1] 17742 1556267610.695144441 1556267613.595471794
[Project1] 17740 1556267610.405662588 1556267619.054863255
[Project1] 17743 1556267619.055766535 1556267630.625824266
[Project1] 17741 1556267630.626792157 1556267651.026717751
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/CCZAiPP.png)


#### 測試資料5
資料內容：
```
PSJF
5
P1 100 100
P2 100 4000
P3 200 200
P4 200 4000
P5 200 7000
```
執行結果：
```
P1 17921
P3 17923
P2 17922
P4 17924
P5 17925
[Project1] 17921 1556273200.242746796 1556273200.528447951
[Project1] 17923 1556273200.534993404 1556273201.109025057
[Project1] 17922 1556273200.529555476 1556273212.639375820
[Project1] 17924 1556273212.640065418 1556273224.173449223
[Project1] 17925 1556273224.174105085 1556273244.520677485
```
與理論解的比較圖（上半為理論解，下半為我們的結果）：
![](https://i.imgur.com/H3FGcXA.png)


## Conclusion

在實際執行時，我們發現若使用三種priority給予不同狀態的程序，可以讓程序間的銜接最好，同時也較容易進行排程管理。priority的分級如下：
1. 執行中的process：最高的priority
2. 下一個準備執行的prcess：第二高的priority
3. 其餘的process：最低的priority

經過分析，我們認為理論與實際的差異主要來自於以下三點：
1. scheduler 除了排程外，還有其他執行所花費的時間，例如動態調整各process的priority、執行新的process與資料結構的操作等。
2. 同樣執行 1000000 次迴圈，但 CPU 所花費的時間不完全一樣，除了各process執行迴圈的單位時間不相同外，scheduler的計時器也不會完全和process的同步，進而造成排程上的誤差。
3. 因為電腦上不只有我們的程序，所以有可能我們的程序因為context switch而被中斷，然而我們的計時不會跟著扣掉，因此造成程序執行時間增加。
