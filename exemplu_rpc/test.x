struct numbers {
    int a;
    int b;
};

program SUM_PROG {
    version SUM_VERS {
        int SUM(struct numbers) = 1;
        int SUB(struct numbers) = 2;
    } = 1;
} = 0x30000002;