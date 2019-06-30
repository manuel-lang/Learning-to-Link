# Learning-to-Link

This repository contains the source code for the paper `Learning to Link` by Maria-Florina Balcan, Travis Dick and Manuel Lang. 
We solve the problem of finding the best linkage strategy for bottom up clustering algorithms. Also, we learn a weighted 
combination of metrics to find the best feature representation of the given dataset.

## Contributors
- [Travis Dick](https://github.com/TravisBarryDick)
- [Manuel Lang](https://github.com/manu183)

## Running the code

We provide two scripts for running the experiments, one for the algorithm selection and the other for learning the metric.
To create the executables, please use the CMakeLists.txt by running `cmake .` and `make` in the root directory.

### Algorithm selection

```
./AlphaLinkage <argument list>
```

You can use the following command line arguments.

| Argument      | Description   |
| ------------- | ------------- |
| --help       | Display usage options              |
| --batch      | Select the n-th set of the given number of points for each class|
| --folder     | Evaluate all csv files in the given folder |
| --input      | Evaluate the given csv file |
| --job        | Create an MNIST job (e.g. --job 0 will run labels 0,1,2,3,4)|
| --labels     | Select the CSV encoded labels only (e.g. --labels 1,2,4)|
| --majority   | Use Majority distance instead of Hamming distance|
| --noaverage  | Directly output the results without averaging them over multiple files|
| --output     | Path where the result will be stored|
| --points     | Number of points used for each class|
| --verbose    | Output the ranges to the console|
| --averagecomplete      | (Default) Interpolate between average and complete linkage|
| --singleaverage        | Interpolate between single and average linkage|
| --singlecomplete       | Interpolate between single and complete linkage|

Example:
```
./AlphaLinkage --input ./input.csv --output ./output.csv --points 200 --singlecomplete
```
This command will read 200 points for each class from the file `input.csv`, interpolate between single and complete linkage, and output the results to `output.csv`.

### Optimizing the Metric

```
./DistanceLearning <parameter_string> <input1.csv> <input2.csv>
```

The parameter string is a list of the four following characters.

| Index      | Description   |
| ------------- | ------------- |
| 1      | Cost function: 'm' for Majority or 'h' for Hamming cost              |
| 2     | Linkage type: 's' for single linkage or 'c' for complete linkage|
| 3    | File type of the first file: 'd' for distance matrix, 'c' for cosine distance (raw features) or 'e' for the Euclidean distance (raw features) |
| 4    | File type of the second file: 'd' for distance matrix, 'c' for cosine distance (raw features) or 'e' for the Euclidean distance (raw features) |

Example:

```
./DistanceLearning hcee input1.csv input2.csv
```

This example loads the two files `input1.csv` and `input2.csv`, learns the best metric using complete linkage and the Hamming cost, and output the results to the console. The pointwise distances are calculated using the Euclidean distances in both cases.
