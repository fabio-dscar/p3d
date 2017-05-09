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

        void reset() {
            currDim = 0;
        }

        void allocArray(uint32 size) {
            arrays.emplace_back(size);
        }

        const std::vector<T>& nextArray() {
            return arrays[currDim++];
        }

        T& next(uint32 sample) {
            return arrays[currDim++][sample];
        }
    };

}