// Golden helper pixels from the native screen regions, not Canvas drawing.
struct HelperReference {int kind,width,height;uint32_t rows[7];};
static const HelperReference helperReferences[]={
    {0,10,7,{72,204,462,975,462,204,72}}, // p70
    {1,17,7,{8200,25484,57422,123279,57358,24844,8200}}, // p115
    {2,5,5,{14,31,31,31,14}}, // p121
    {3,13,5,{3598,7953,7953,7953,3598}}, // p70
    {4,21,5,{921102,2035985,2035985,2035985,921102}}, // p80
    {3,13,5,{3598,7953,7953,7953,3598}}, // p99
    {5,16,7,{4488,12876,28750,61583,28814,12300,4232}}, // p136
};
