namespace Photon {

    template<typename T>
    struct SampleArrays {
    public:
        uint32 currDim;
        std::vector<std::vector<T>> arrays;

        SampleArrays() : currDim(0) {}

        bool hasNext() {
            return currDim < arrays.size();
        }

        bool hasNext(uint32 numSamples) {
            uint32 i = currDim;
            while (i < arrays.size() && 
                   arrays[i++].size() != numSamples)
                ;

            return i < arrays.size();
        }

        void reset() {
            currDim = 0;
        }

        void allocArray(uint32 size) {
            arrays.emplace_back(size);
        }

        const std::vector<T>& nextArray(uint32 numSamples) {
            return arrays[currDim++];
        }

        T& next(uint32 sample) {
            return arrays[currDim++][sample];
        }
    };

}