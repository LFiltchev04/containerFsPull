


class baseTreeTransition{
    std::string src;
    std::string dst;

    uint32_t accessCount;
    uint32_t hitCount;
    uint32_t guessCount;

    baseTreeTransition(std::string s, std::string d);
    void recordTransition(bool hit);

    double getGuessRate();
    uint32_t *getAccessCount();
};