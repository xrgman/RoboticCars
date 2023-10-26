/**********************************************************************
* Written by WAVToCode
* Date:				Mon Oct 16 06:18:34 PM
* FileName:			beep.h
* Interleaved:		N/A
* Signed:			No
* No. of channels:	1
* No. of samples:	4829
* Bits/Sample:		8
**********************************************************************/

const uint16_t nrElementsBeep = 4829;

uint8_t data_beep[nrElementsBeep] = { 64, 
70, 77, 84, 91, 98, 105, 105, 98, 
91, 84, 77, 70, 63, 56, 49, 42, 
35, 28, 21, 23, 30, 37, 44, 51, 
58, 65, 72, 79, 86, 93, 100, 107, 
103, 96, 89, 82, 75, 68, 61, 54, 
47, 40, 33, 26, 19, 25, 32, 39, 
46, 53, 60, 67, 74, 81, 88, 95, 
102, 108, 101, 94, 87, 80, 73, 66, 
59, 52, 45, 38, 31, 24, 20, 27, 
34, 41, 48, 55, 62, 69, 76, 83, 
90, 97, 104, 106, 99, 92, 85, 78, 
71, 64, 57, 50, 43, 36, 29, 22, 
22, 29, 36, 43, 50, 57, 64, 71, 
78, 85, 92, 99, 106, 104, 97, 90, 
83, 76, 69, 62, 55, 48, 41, 34, 
27, 20, 24, 31, 38, 45, 52, 59, 
66, 73, 80, 87, 94, 101, 108, 102, 
95, 88, 81, 74, 67, 60, 53, 46, 
39, 32, 25, 19, 26, 33, 40, 47, 
54, 61, 68, 75, 82, 89, 96, 103, 
107, 100, 93, 86, 79, 72, 65, 58, 
51, 44, 37, 30, 23, 21, 28, 35, 
42, 49, 56, 63, 70, 77, 84, 91, 
98, 105, 105, 98, 91, 84, 77, 70, 
63, 57, 50, 43, 36, 29, 22, 23, 
30, 37, 44, 51, 58, 65, 72, 79, 
86, 93, 99, 107, 104, 97, 90, 82, 
76, 68, 62, 55, 47, 41, 33, 27, 
19, 25, 31, 39, 45, 53, 59, 67, 
74, 81, 88, 94, 102, 108, 102, 94, 
88, 81, 73, 67, 60, 52, 46, 39, 
31, 25, 19, 27, 34, 40, 48, 54, 
62, 68, 76, 82, 89, 97, 103, 107, 
99, 93, 86, 78, 72, 65, 58, 50, 
44, 36, 30, 23, 22, 29, 36, 43, 
50, 57, 64, 71, 78, 85, 92, 99, 
106, 105, 98, 91, 84, 77, 70, 63, 
56, 49, 42, 35, 28, 21, 24, 31, 
37, 45, 51, 59, 65, 73, 79, 87, 
93, 101, 107, 103, 95, 89, 82, 75, 
67, 61, 53, 47, 40, 32, 26, 18, 
26, 32, 40, 46, 54, 61, 68, 75, 
82, 89, 96, 103, 108, 101, 93, 87, 
79, 73, 65, 59, 52, 44, 38, 30, 
24, 21, 28, 34, 42, 49, 55, 63, 
69, 77, 83, 91, 97, 105, 106, 98, 
92, 84, 78, 70, 64, 56, 50, 43, 
35, 29, 21, 23, 29, 37, 43, 51, 
57, 64, 71, 78, 85, 92, 99, 106, 
104, 97, 90, 83, 76, 69, 62, 55, 
48, 41, 34, 27, 20, 24, 31, 38, 
45, 52, 59, 66, 73, 80, 87, 94, 
101, 108, 102, 95, 88, 81, 74, 67, 
60, 53, 46, 39, 32, 25, 19, 26, 
33, 40, 47, 54, 61, 68, 75, 82,
89, 96, 103, 107, 100, 93, 86, 79, 
72, 65, 58, 51, 44, 37, 30, 23, 
21, 28, 35, 42, 49, 56, 63, 70, 
77, 84, 91, 98, 105, 105, 98, 91, 
84, 77, 70, 63, 56, 49, 42, 35, 
28, 21, 23, 30, 37, 44, 51, 58, 
65, 72, 79, 86, 93, 100, 107, 103, 
96, 89, 82, 75, 68, 61, 54, 47, 
40, 33, 26, 19, 25, 32, 39, 46, 
53, 60, 67, 74, 81, 88, 95, 102, 
108, 101, 94, 87, 80, 73, 66, 59, 
52, 45, 38, 31, 24, 20, 27, 34, 
41, 48, 55, 62, 69, 76, 83, 90, 
97, 104, 106, 99, 92, 85, 78, 71, 
64, 57, 50, 43, 36, 29, 22, 22, 
29, 36, 43, 50, 57, 64, 71, 78, 
85, 92, 99, 106, 104, 97, 90, 83, 
76, 69, 63, 55, 49, 41, 35, 27, 
21, 23, 31, 37, 45, 51, 59, 65, 
73, 79, 87, 94, 100, 108, 102, 96, 
89, 82, 75, 68, 61, 53, 47, 39, 
33, 25, 19, 26, 32, 40, 46, 54, 
60, 68, 74, 82, 88, 96, 102, 108, 
100, 94, 86, 80, 73, 66, 59, 51, 
45, 37, 31, 23, 21, 28, 34, 42, 
48, 56, 62, 70, 76, 84, 91, 97, 
105, 105, 99, 91, 85, 77, 71, 63, 
57, 49, 43, 36, 29, 22, 22, 30, 
36, 44, 50, 58, 64, 72, 79, 85, 
93, 99, 107, 104, 97, 90, 82, 76, 
68, 62, 54, 48, 40, 34, 26, 20, 
24, 32, 38, 46, 52, 60, 66, 74, 
80, 88, 94, 102, 108, 102, 94, 88, 
81, 73, 67, 60, 52, 46, 38, 32, 
24, 20, 26, 34, 40, 48, 55, 61, 
69, 75, 83, 90, 96, 104, 106, 100, 
93, 85, 79, 71, 65, 57, 51, 43, 
37, 30, 22, 22, 28, 36, 43, 49, 
57, 64, 71, 78, 84, 92, 98, 106, 
104, 98, 91, 84, 77, 69, 63, 55, 
49, 41, 35, 27, 21, 24, 31, 38, 
44, 51, 58, 65, 72, 79, 86, 93, 
100, 107, 103, 96, 89, 82, 75, 68, 
61, 54, 47, 40, 33, 26, 19, 25, 
32, 39, 46, 53, 60, 67, 74, 81, 
88, 95, 102, 108, 101, 94, 87, 80, 
73, 66, 59, 52, 45, 38, 31, 24, 
20, 27, 34, 41, 48, 55, 62, 69, 
76, 83, 90, 97, 104, 106, 99, 92, 
85, 78, 71, 64, 57, 50, 43, 36, 
29, 22, 22, 29, 36, 43, 50, 57, 
64, 71, 78, 85, 92, 99, 106, 104, 
97, 90, 83, 76, 69, 62, 55, 48, 
41, 34, 27, 20, 24, 31, 38, 45, 
52, 59, 66, 73, 80, 87, 94, 101, 
108, 102, 95, 88, 81, 74, 67, 60, 
53, 46, 39, 32, 25, 19, 26, 33, 
40, 47, 54, 61, 68, 75, 82, 89, 
96, 103, 107, 100, 93, 86, 79, 72, 
65, 58, 51, 44, 37, 30, 23, 21, 
28, 35, 42, 49, 56, 63, 70, 77, 
84, 91, 98, 105, 105, 98, 91, 84,
77, 70, 63, 56, 49, 42, 35, 28, 
21, 23, 30, 37, 44, 51, 58, 65, 
72, 79, 86, 93, 100, 107, 103, 96, 
89, 82, 76, 68, 62, 54, 48, 41, 
34, 27, 20, 25, 32, 39, 46, 53, 
60, 66, 74, 80, 88, 94, 102, 108, 
102, 94, 88, 80, 74, 67, 59, 53, 
45, 39, 32, 24, 20, 26, 33, 41, 
47, 55, 61, 69, 76, 83, 89, 97, 
103, 107, 99, 93, 86, 79, 71, 65, 
57, 51, 43, 36, 30, 22, 22, 28, 
36, 42, 50, 56, 63, 71, 77, 85, 
91, 98, 106, 104, 98, 91, 84, 77, 
70, 63, 55, 49, 41, 35, 27, 21, 
23, 30, 38, 44, 52, 59, 65, 73, 
79, 87, 93, 101, 107, 103, 95, 89, 
81, 75, 67, 61, 54, 46, 40, 32, 
26, 18, 26, 32, 40, 46, 54, 60, 
68, 74, 82, 88, 96, 103, 107, 101, 
93, 87, 79, 73, 65, 59, 51, 45, 
37, 31, 23, 21, 27, 35, 41, 49, 
56, 63, 70, 77, 84, 90, 98, 104, 
106, 98, 92, 84, 78, 71, 63, 57, 
49, 43, 35, 29, 21, 23, 30, 36, 
44, 50, 57, 65, 72, 78, 86, 92, 
100, 106, 104, 96, 90, 82, 76, 68, 
62, 54, 48, 40, 34, 26, 20, 25, 
31, 39, 45, 52, 59, 66, 73, 80, 
87, 94, 101, 108, 102, 95, 88, 81, 
74, 67, 60, 53, 46, 39, 32, 25, 
19, 26, 33, 40, 47, 54, 61, 68, 
75, 82, 89, 96, 103, 107, 100, 93, 
86, 79, 72, 65, 58, 51, 44, 37, 
30, 23, 21, 28, 35, 42, 49, 56, 
63, 70, 77, 84, 91, 98, 105, 105, 
98, 91, 84, 77, 70, 63, 56, 49, 
42, 35, 28, 21, 23, 30, 37, 44, 
51, 58, 65, 72, 79, 86, 93, 100, 
107, 103, 96, 89, 82, 75, 68, 61, 
54, 47, 40, 33, 26, 19, 25, 32, 
39, 46, 53, 60, 67, 74, 81, 88, 
95, 102, 108, 101, 94, 87, 80, 73, 
66, 59, 52, 45, 38, 31, 24, 20, 
27, 34, 41, 48, 55, 62, 69, 76, 
83, 90, 97, 104, 106, 99, 92, 85, 
78, 71, 64, 57, 50, 43, 36, 29, 
22, 22, 29, 36, 43, 50, 57, 64, 
71, 78, 85, 92, 99, 106, 104, 97, 
90, 83, 76, 69, 62, 55, 48, 41, 
34, 27, 20, 24, 31, 38, 45, 52, 
59, 66, 73, 80, 87, 94, 101, 108, 
102, 95, 88, 81, 75, 67, 61, 54, 
46, 40, 32, 26, 18, 26, 32, 40, 
47, 53, 61, 67, 75, 81, 89, 95, 
103, 107, 101, 93, 87, 79, 73, 66, 
58, 52, 44, 38, 31, 23, 21, 27, 
35, 41, 49, 56, 63, 70, 76, 84, 
90, 98, 104, 106, 98, 92, 84, 78, 
70, 64, 56, 50, 42, 36, 28, 22, 
22, 30, 36, 44, 50, 58, 64, 72, 
79, 85, 93, 99, 107, 104, 97, 90, 
83, 76, 69, 62, 55, 48, 41, 33, 
27, 19, 25, 31, 39, 45, 53, 59, 
67, 73, 81, 88, 95, 101, 109, 102, 
95, 88, 80, 73, 67, 59, 53, 45,
39, 32, 25, 19, 27, 33, 41, 48, 
55, 62, 69, 75, 83, 89, 97, 103, 
107, 99, 93, 85, 79, 71, 65, 58, 
50, 44, 37, 29, 23, 21, 29, 35, 
43, 49, 57, 63, 71, 77, 85, 91, 
99, 105, 105, 98, 90, 84, 76, 70, 
62, 56, 49, 41, 35, 27, 21, 24, 
31, 37, 45, 51, 59, 66, 73, 79, 
87, 94, 101, 108, 102, 96, 88, 82, 
74, 68, 60, 54, 46, 40, 32, 26, 
18, 26, 33, 39, 46, 53, 60, 67, 
74, 81, 88, 95, 102, 108, 101, 94, 
87, 80, 73, 66, 59, 52, 45, 38, 
31, 24, 20, 27, 34, 41, 48, 55, 
62, 69, 76, 83, 90, 97, 104, 106, 
99, 92, 85, 78, 71, 64, 57, 50, 
43, 36, 29, 22, 22, 29, 36, 43, 
50, 57, 64, 71, 78, 85, 92, 99, 
106, 104, 97, 90, 83, 76, 69, 62, 
55, 48, 41, 34, 27, 20, 24, 31, 
38, 45, 52, 59, 66, 73, 80, 87, 
94, 101, 108, 102, 95, 88, 81, 74, 
67, 60, 53, 46, 39, 32, 25, 19, 
26, 33, 40, 47, 54, 61, 68, 75, 
82, 89, 96, 103, 107, 100, 93, 86, 
79, 72, 65, 58, 51, 44, 37, 30, 
23, 21, 28, 35, 42, 49, 56, 63, 
70, 77, 84, 91, 98, 105, 105, 98, 
91, 84, 77, 70, 63, 56, 49, 42, 
35, 28, 21, 23, 30, 37, 44, 51, 
58, 65, 72, 79, 86, 93, 100, 107, 
103, 96, 89, 82, 75, 68, 61, 54, 
47, 40, 33, 26, 19, 25, 32, 39, 
46, 53, 60, 67, 74, 81, 88, 95, 
102, 108, 101, 95, 87, 81, 73, 67, 
59, 53, 46, 38, 32, 24, 20, 27, 
33, 41, 47, 55, 62, 68, 76, 83, 
89, 97, 103, 107, 99, 93, 85, 79, 
72, 65, 57, 51, 43, 37, 30, 23, 
21, 29, 35, 43, 50, 56, 64, 70, 
78, 84, 92, 99, 105, 105, 97, 91, 
83, 77, 69, 63, 56, 48, 42, 34, 
28, 21, 23, 31, 37, 45, 51, 59, 
66, 72, 80, 86, 94, 101, 108, 103, 
96, 89, 82, 75, 67, 61, 53, 47, 
40, 32, 26, 18, 26, 33, 40, 46, 
54, 60, 68, 74, 82, 88, 96, 103, 
107, 101, 93, 87, 80, 73, 66, 59, 
51, 45, 38, 30, 24, 21, 27, 35, 
41, 49, 55, 63, 70, 77, 83, 91, 
97, 105, 105, 98, 92, 84, 78, 70, 
64, 56, 49, 43, 35, 29, 21, 23, 
29, 37, 43, 51, 57, 65, 71, 79, 
85, 93, 100, 106, 104, 96, 90, 83, 
76, 68, 62, 54, 48, 40, 33, 27, 
19, 25, 32, 38, 46, 52, 60, 67, 
73, 81, 87, 94, 102, 108, 102, 94, 
88, 80, 74, 67, 59, 53, 46, 38, 
32, 24, 20, 26, 33, 40, 47, 54, 
61, 68, 75, 82, 89, 96, 103, 107, 
100, 93, 86, 79, 72, 65, 58, 51, 
44, 37, 30, 23, 21, 28, 35, 42, 
49, 56, 63, 70, 77, 84, 91, 98, 
105, 105, 98, 91, 84, 77, 70, 63, 
56, 49, 42, 35, 28, 21, 23, 30, 
37, 44, 51, 58, 65, 72, 79, 86, 
93, 100, 107, 103, 96, 89, 82, 75, 
68, 61, 54, 47, 40, 33, 26, 19, 
25, 32, 39, 46, 53, 60, 67, 74, 
81, 88, 95, 102, 108, 101, 94, 87, 
80, 73, 66, 59, 52, 45, 38, 31, 
24, 20, 27, 34, 41, 48, 55, 62, 
69, 76, 83, 90, 97, 104, 106, 99, 
92, 85, 78, 71, 64, 57, 50, 43, 
36, 29, 22, 22, 29, 36, 43, 50, 
57, 64, 71, 78, 85, 92, 99, 106, 
104, 97, 90, 83, 76, 69, 62, 55, 
48, 41, 34, 27, 20, 24, 31, 38, 
45, 52, 59, 66, 73, 80, 87, 94, 
101, 108, 102, 95, 88, 81, 74, 67, 
60, 53, 46, 39, 32, 25, 19, 26, 
33, 40, 47, 54, 61, 68, 75, 82, 
89, 96, 103, 107, 101, 94, 86, 80, 
72, 66, 58, 52, 45, 37, 31, 23, 
21, 28, 34, 42, 48, 56, 63, 69, 
77, 83, 91, 98, 104, 106, 98, 92, 
84, 78, 70, 64, 56, 50, 42, 36, 
28, 22, 22, 29, 37, 43, 51, 57, 
65, 72, 78, 86, 92, 100, 106, 104, 
96, 90, 82, 76, 68, 62, 54, 48, 
41, 33, 27, 19, 25, 31, 39, 45, 
53, 59, 67, 73, 81, 87, 95, 101, 
109, 101, 95, 87, 80, 74, 67, 59, 
53, 45, 39, 32, 24, 20, 26, 34, 
40, 48, 55, 62, 69, 76, 82, 90, 
96, 104, 107, 99, 93, 85, 79, 71, 
65, 57, 51, 43, 37, 30, 23, 21, 
29, 35, 43, 49, 57, 63, 71, 78, 
85, 92, 99, 105, 105, 97, 91, 83,
77, 70, 62, 56, 48, 42, 34, 28, 
20, 24, 31, 37, 45, 52, 58, 66, 
72, 80, 87, 93, 101, 107, 103, 95, 
89, 81, 75, 67, 61, 53, 47, 39, 
33, 26, 18, 26, 33, 39, 47, 53, 
61, 67, 75, 81, 89, 95, 103, 107, 
101, 93, 87, 79, 73, 65, 59, 52, 
44, 38, 30, 24, 21, 27, 34, 41, 
48, 55, 62, 69, 76, 83, 90, 97, 
104, 106, 99, 92, 85, 78, 71, 64, 
57, 50, 43, 36, 29, 22, 22, 29, 
36, 43, 50, 57, 64, 71, 78, 85, 
92, 99, 106, 104, 97, 90, 83, 76, 
69, 62, 55, 48, 41, 34, 27, 20, 
24, 31, 38, 45, 52, 59, 66, 73, 
80, 87, 94, 101, 108, 102, 95, 88, 
81, 74, 67, 60, 53, 46, 39, 32, 
25, 19, 26, 33, 40, 47, 54, 61, 
68, 75, 82, 89, 96, 103, 107, 100, 
93, 86, 79, 72, 65, 58, 51, 44, 
37, 30, 23, 21, 28, 35, 42, 49, 
56, 63, 70, 77, 84, 91, 98, 105, 
105, 98, 91, 84, 77, 70, 63, 56, 
49, 42, 35, 28, 21, 23, 30, 37, 
44, 51, 58, 65, 72, 79, 86, 93, 
100, 107, 103, 96, 89, 82, 75, 68, 
61, 54, 47, 40, 33, 26, 19, 25, 
32, 39, 46, 53, 60, 67, 74, 81, 
88, 95, 102, 108, 101, 94, 87, 80, 
73, 66, 59, 52, 45, 38, 31, 24, 
20, 27, 34, 41, 48, 55, 62, 69, 
76, 83, 90, 97, 104, 106, 100, 92, 
86, 78, 72, 64, 58, 51, 43, 37, 
29, 23, 21, 29, 35, 43, 49, 57, 
63, 71, 77, 85, 91, 99, 106, 104, 
98, 90, 84, 76, 70, 63, 55, 49, 
42, 34, 28, 20, 24, 30, 38, 44, 
52, 59, 65, 73, 79, 87, 93, 101, 
107, 103, 95, 89, 81, 75, 67, 61, 
53, 47, 39, 33, 25, 19, 25, 33, 
39, 47, 54, 60, 68, 74, 82, 89, 
95, 103, 107, 101, 93, 87, 79, 73, 
65, 59, 51, 45, 37, 31, 24, 20, 
28, 34, 42, 48, 56, 62, 70, 77, 
83, 91, 97, 105, 106, 99, 91, 85, 
77, 71, 63, 57, 49, 43, 35, 29, 
22, 23, 30, 36, 44, 50, 58, 65, 
71, 79, 85, 93, 100, 106, 104, 96, 
90, 82, 76, 68, 61, 55, 47, 41, 
33, 27, 19, 25, 31, 39, 45, 53, 
59, 67, 73, 81, 87, 95, 102, 108, 
102, 94, 88, 80, 74, 67, 59, 53, 
45, 39, 32, 24, 20, 26, 34, 40, 
48, 54, 62, 68, 76, 82, 90, 96, 
104, 106, 100, 92, 86, 79, 72, 65, 
58, 50, 44, 37, 30, 22, 21, 28, 
35, 42, 49, 56, 63, 70, 77, 84, 
91, 98, 105, 105, 98, 91, 84, 77, 
70, 63, 56, 49, 42, 35, 28, 21, 
23, 30, 37, 44, 51, 58, 65, 72, 
79, 86, 93, 100, 107, 103, 96, 89, 
82, 75, 68, 61, 54, 47, 40, 33, 
26, 19, 25, 32, 39, 46, 53, 60, 
67, 74, 81, 88, 95, 102, 108, 101, 
94, 87, 80, 73, 66, 59, 52, 45, 
38, 31, 24, 20, 27, 34, 41, 48, 
55, 62, 69, 76, 83, 90, 97, 104, 
106, 99, 92, 85, 78, 71, 64, 57, 
50, 43, 36, 29, 22, 22, 29, 36, 
43, 50, 57, 64, 71, 78, 85, 92, 
99, 106, 104, 97, 90, 83, 76, 69, 
62, 55, 48, 41, 34, 27, 20, 24, 
31, 38, 45, 52, 59, 66, 73, 80, 
87, 94, 101, 108, 102, 95, 88, 81, 
74, 67, 60, 53, 46, 39, 32, 25, 
19, 26, 33, 40, 47, 54, 61, 68, 
75, 82, 89, 96, 103, 107, 100, 93, 
86, 79, 72, 65, 58, 51, 44, 37, 
30, 23, 21, 28, 35, 42, 49, 56, 
63, 70, 77, 84, 91, 98, 105, 106, 
99, 92, 84, 78, 70, 64, 57, 50, 
42, 36, 28, 22, 22, 30, 36, 44, 
51, 58, 65, 72, 79, 85, 93, 99, 
107, 103, 97, 90, 82, 76, 69, 61, 
55, 48, 40, 34, 26, 20, 24, 32, 
38, 46, 52, 60, 67, 74, 80, 88, 
94, 102, 109, 102, 94, 88, 80, 74, 
66, 60, 53, 45, 39, 31, 25, 19, 
27, 33, 41, 47, 55, 61, 69, 75, 
83, 90, 97, 104, 107, 100, 92, 86, 
78, 72, 64, 58, 51, 44, 37, 30, 
22, 22, 28, 36, 43, 50, 57, 63, 
71, 77, 85, 91, 99, 105, 105, 97, 
91, 83, 77, 69, 63, 55, 49, 42, 
34, 28, 20, 24, 31, 37, 45, 51, 
59, 65, 73, 79, 87, 93, 101, 107, 
103, 96, 88, 82, 74, 68, 61, 53, 
47, 39, 33, 25, 19, 25, 33, 39, 
47, 53, 61, 68, 74, 82, 89, 95, 
103, 107, 101, 93, 87, 79, 73, 65, 
59, 52, 44, 38, 30, 24, 20, 28, 
34, 42, 48, 55, 63, 69, 77, 83, 
91, 97, 105, 106, 98, 92, 84, 78, 
71, 63, 57, 50, 42, 36, 28, 22, 
22, 29, 36, 43, 50, 57, 64, 71, 
78, 85, 92, 99, 106, 104, 97, 90, 
83, 76, 69, 62, 55, 48, 41, 34, 
27, 20, 24, 31, 38, 45, 52, 59, 
66, 73, 80, 87, 94, 101, 108, 102, 
95, 88, 81, 74, 67, 60, 53, 46, 
39, 32, 25, 19, 26, 33, 40, 47, 
54, 61, 68, 75, 82, 89, 96, 103, 
107, 100, 93, 86, 79, 72, 65, 58, 
51, 44, 37, 30, 23, 21, 28, 35, 
42, 49, 56, 63, 70, 77, 84, 91, 
98, 105, 105, 98, 91, 84, 77, 70, 
63, 56, 49, 42, 35, 28, 21, 23, 
30, 37, 44, 51, 58, 65, 72, 79, 
86, 93, 100, 107, 103, 96, 89, 82, 
75, 68, 61, 54, 47, 40, 33, 26, 
19, 25, 32, 39, 46, 53, 60, 67, 
74, 81, 88, 95, 102, 108, 101, 94, 
87, 80, 73, 66, 59, 52, 45, 38, 
31, 24, 20, 27, 34, 41, 48, 55, 
62, 69, 76, 83, 90, 97, 104, 106, 
99, 92, 85, 78, 71, 64, 57, 50, 
43, 36, 29, 22, 22, 29, 36, 43, 
50, 57, 64, 71, 78, 85, 92, 99, 
105, 105, 97, 91, 84, 77, 69, 63, 
55, 49, 41, 35, 28, 21, 24, 30, 
38, 45, 52, 59, 66, 72, 80, 86, 
94, 101, 107, 103, 95, 89, 81, 75, 
68, 60, 54, 47, 40, 33, 26, 18, 
26, 33, 39, 47, 53, 61, 67, 75, 
82, 89, 95, 103, 107, 101, 93, 87, 
80, 72, 66, 58, 52, 44, 38, 30, 
24, 20, 28, 34, 41, 49, 55, 63, 
69, 77, 84, 90, 98, 104, 105, 99, 
91, 84, 77, 70, 63, 57, 50, 43, 
36, 29, 22, 23, 30, 37, 44, 51, 
58, 64, 71, 78, 85, 92, 99, 106, 
103, 96, 89, 82, 75, 68, 62, 55, 
48, 41, 34, 27, 20, 25, 32, 39, 
46, 53, 60, 66, 73, 80, 87, 94, 
101, 108, 101, 94, 87, 80, 73, 66, 
60, 53, 46, 39, 32, 25, 20, 27, 
34, 41, 48, 55, 62, 68, 75, 82, 
89, 96, 103, 106, 99, 92, 85, 78, 
71, 64, 58, 51, 44, 37, 30, 23, 
22, 29, 36, 43, 50, 57, 63, 70, 
77, 84, 91, 98, 105, 104, 97, 90, 
83, 76, 69, 63, 56, 49, 42, 36, 
28, 22, 23, 31, 37, 45, 51, 59, 
64, 72, 78, 86, 92, 100, 106, 103, 
95, 89, 81, 75, 67, 62, 55, 48, 
41, 33, 27, 19, 26, 32, 40, 46, 
54, 61, 67, 74, 80, 88, 94, 102, 
107, 101, 93, 87, 79, 73, 65, 60, 
52, 46, 38, 32, 24, 21, 27, 35, 
41, 49, 55, 62, 69, 75, 83, 90, 
96, 104, 106, 98, 92, 84, 78, 71, 
63, 58, 50, 44, 36, 30, 22, 23, 
29, 37, 44, 50, 58, 63, 71, 78, 
84, 92, 98, 106, 103, 97, 90, 82, 
76, 69, 63, 56, 49, 41, 35, 27, 
21, 24, 31, 39, 45, 53, 59, 66, 
73, 80, 87, 93, 101, 107, 102, 94, 
88, 80, 74, 66, 61, 53, 47, 40, 
32, 26, 20, 26, 34, 40, 48, 54, 
62, 68, 74, 82, 88, 96, 102, 107, 
99, 93, 86, 78, 72, 64, 59, 51, 
45, 38, 30, 24, 21, 29, 35, 43, 
50, 56, 64, 69, 77, 83, 91, 97, 
105, 104, 98, 91, 83, 77, 69, 64, 
57, 49, 43, 35, 29, 21, 24, 30, 
38, 45, 51, 59, 64, 72, 78, 86, 
92, 99, 106, 103, 96, 89, 82, 75, 
68, 62, 55, 48, 41, 34, 27, 20, 
25, 32, 39, 46, 53, 60, 66, 73, 
80, 87, 94, 101, 108, 101, 94, 87, 
80, 73, 66, 60, 53, 46, 39, 32, 
25, 20, 27, 34, 41, 48, 55, 62, 
68, 75, 82, 89, 96, 103, 106, 99, 
92, 85, 78, 71, 64, 58, 51, 44, 
37, 30, 23, 22, 29, 36, 43, 50, 
57, 64, 70, 77, 84, 91, 98, 105, 
104, 97, 90, 83, 76, 69, 63, 56, 
49, 42, 35, 28, 21, 24, 31, 38, 
45, 52, 59, 65, 72, 79, 86, 93, 
100, 107, 102, 95, 88, 81, 74, 67, 
61, 54, 47, 40, 33, 26, 19, 26, 
33, 40, 47, 54, 61, 67, 74, 81, 
88, 95, 102, 107, 100, 93, 86, 79, 
72, 65, 59, 52, 45, 38, 31, 24, 
21, 28, 35, 42, 49, 56, 63, 69, 
76, 83, 90, 97, 104, 105, 98, 91, 
84, 77, 70, 64, 57, 50, 43, 36, 
29, 22, 23, 30, 37, 44, 51, 58, 
64, 71, 78, 85, 92, 99, 106, 103, 
96, 89, 82, 75, 68, 62, 55, 48, 
41, 35, 27, 21, 24, 32, 39, 46, 
52, 60, 65, 73, 79, 87, 94, 100, 
108, 101, 95, 88, 80, 74, 66, 61, 
53, 47, 40, 33, 25, 20, 26, 34, 
41, 48, 55, 62, 68, 74, 82, 88, 
96, 102, 107, 99, 93, 85, 79, 71, 
65, 59, 51, 45, 38, 31, 24, 22, 
29, 35, 43, 49, 57, 63, 70, 76, 
84, 91, 97, 105, 104, 98, 90, 84, 
76, 70, 63, 57, 49, 43, 36, 29, 
21, 24, 30, 38, 45, 51, 59, 65, 
71, 79, 85, 93, 100, 107, 102, 96, 
88, 82, 75, 68, 62, 54, 48, 40, 
34, 26, 19, 26, 32, 40, 47, 53, 
61, 66, 74, 80, 88, 95, 101, 108, 
101, 93, 87, 79, 73, 65, 60, 52, 
46, 38, 32, 24, 21, 27, 35, 41, 
49, 55, 63, 68, 76, 82, 90, 97, 
104, 105, 99, 91, 85, 77, 71, 63, 
58, 50, 44, 36, 30, 23, 22, 30, 
36, 44, 51, 58, 64, 71, 78, 84, 
92, 98, 106, 104, 96, 90, 82, 76, 
69, 62, 56, 48, 42, 34, 28, 21, 
24, 32, 39, 45, 53, 59, 66, 72, 
80, 86, 93, 100, 107, 102, 95, 88, 
81, 74, 67, 61, 54, 47, 40, 33, 
26, 19, 26, 33, 40, 47, 54, 61, 
67, 74, 81, 88, 95, 102, 107, 100, 
93, 86, 79, 72, 65, 59, 52, 45, 
38, 31, 24, 21, 28, 35, 42, 49, 
56, 63, 69, 76, 83, 90, 97, 104, 
105, 98, 91, 84, 77, 70, 63, 57, 
50, 43, 36, 29, 22, 23, 30, 37, 
44, 51, 58, 64, 71, 78, 85, 92, 
99, 106, 103, 96, 89, 82, 75, 68, 
62, 55, 48, 41, 34, 27, 20, 25, 
32, 39, 46, 53, 60, 66, 73, 80, 
87, 94, 101, 108, 101, 94, 87, 80, 
73, 66, 60, 53, 46, 39, 32, 25, 
20, 27, 34, 41, 48, 55, 62, 68, 
75, 82, 89, 96, 103, 106, 99, 92, 
85, 78, 71, 64, 58, 51, 44, 37, 
30, 23, 22, 29, 36, 43, 50, 57, 
63, 70, 77, 84, 91, 98, 105, 104, 
97, 90, 83, 76, 69, 63, 56, 49, 
42, 35, 28, 21, 24, 31, 38, 45, 
52, 59, 65, 72, 79, 86, 93, 100, 
107, 102, 95, 88, 81, 74, 67, 61, 
54, 48, 40, 34, 26, 20, 26, 33, 
40, 47, 54, 60, 67, 73, 81, 87, 
94, 102, 107, 101, 93, 87, 79, 72, 
66, 59, 53, 45, 39, 31, 25, 20, 
28, 34, 42, 49, 55, 63, 68, 76, 
83, 89, 96, 104, 105, 99, 91, 85, 
77, 71, 63, 58, 50, 44, 36, 30, 
23, 22, 30, 36, 44, 50, 58, 64, 
70, 78, 84, 92, 98, 106, 103, 97, 
89, 83, 75, 69, 63, 55, 49, 41, 
35, 28, 20, 25, 32, 39, 46, 52, 
60, 66, 73, 80, 86, 94, 101, 107, 
102, 94, 88, 81, 73, 67, 60, 54, 
46, 40, 32, 26, 19, 27, 33, 41, 
47, 55, 61, 68, 74, 82, 89, 95, 
103, 107, 99, 93, 85, 79, 71, 65, 
58, 52, 44, 38, 31, 23, 22, 29, 
35, 43, 49, 57, 64, 69, 77, 83, 
91, 97, 105, 104, 98, 90, 84, 76, 
70, 63, 57, 49, 43, 35, 29, 21, 
24, 30, 38, 45, 51, 59, 64, 72, 
78, 86, 92, 100, 107, 102, 96, 88, 
82, 74, 68, 61, 55, 48, 40, 34, 
26, 20, 25, 32, 40, 46, 54, 60, 
67, 73, 80, 87, 94, 101, 108, 101, 
94, 87, 80, 73, 66, 60, 53, 46, 
39, 32, 25, 20, 27, 34, 41, 48, 
55, 62, 68, 75, 82, 89, 96, 103, 
106, 99, 92, 85, 78, 71, 64, 58, 
51, 44, 37, 30, 23, 22, 29, 36, 
43, 50, 57, 64, 70, 77, 84, 91, 
98, 105, 104, 97, 90, 83, 76, 69, 
63, 56, 49, 42, 35, 28, 21, 24, 
31, 38, 45, 52, 59, 65, 72, 79, 
86, 93, 100, 107, 102, 95, 88, 81, 
74, 67, 61, 54, 47, 40, 33, 26, 
20, 26, 33, 40, 47, 54, 61, 67, 
74, 81, 88, 95, 102, 106, 100, 93, 
86, 79, 72, 65, 59, 52, 45, 38, 
31, 24, 21, 28, 35, 42, 49, 56, 
63, 69, 76, 83, 90, 97, 104, 105, 
98, 91, 84, 77, 70, 63, 57, 50, 
43, 36, 29, 22, 23, 30, 37, 44, 
51, 58, 64, 71, 78, 85, 92, 99, 
105, 103, 96, 89, 82, 75, 68, 62, 
55, 48, 41, 34, 27, 21, 25, 32, 
39, 46, 53, 60, 66, 73, 80, 87, 
94, 101, 108, 101, 94, 87, 80, 73, 
66, 60, 53, 47, 39, 33, 25, 20, 
26, 34, 40, 48, 54, 62, 67, 75, 
81, 89, 95, 103, 106, 100, 92, 86, 
78, 72, 64, 59, 51, 45, 37, 31, 
24, 22, 28, 36, 43, 50, 57, 64, 
69, 77, 83, 91, 97, 104, 104, 98, 
90, 83, 77, 69, 64, 56, 50, 43, 
35, 29, 22, 24, 30, 38, 44, 52, 
58, 65, 72, 79, 86, 93, 100, 106, 
102, 95, 89, 81, 75, 67, 62, 55, 
47, 41, 33, 27, 20, 26, 33, 40, 
47, 53, 61, 66, 74, 80, 87, 95, 
101, 107, 100, 94, 86, 80, 72, 66, 
60, 52, 46, 38, 32, 25, 21, 28, 
35, 42, 48, 56, 62, 69, 75, 83, 
89, 97, 103, 105, 99, 91, 85, 77, 
71, 63, 58, 51, 43, 37, 29, 23, 
23, 29, 37, 44, 50, 58, 63, 71, 
77, 85, 91, 99, 105, 103, 97, 89, 
83, 76, 68, 63, 55, 49, 41, 35, 
28, 21, 25, 32, 38, 46, 53, 60, 
65, 72, 78, 85, 90, 96, 100, 95, 
88, 82, 77, 71, 66, 61, 57, 52, 
49, 45, 42, 40, 44, 49, 52, 56, 
59, 63, 65, 68, 70, 71, 73, 74, 
75, 72, 70, 68, 66, 65, 64, 63, 
63, 62, 63, 63
}; /* 4824-4828 */