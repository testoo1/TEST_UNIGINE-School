#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>


struct Comparator
{
    bool operator()(const std::pair<int, std::string>& left,
                    const std::pair<int, std::string>& right)
    {
        if (left.first < right.first) return false;
        if (left.first > right.first)  return true;
        return left.second < right.second;
    }
};

void print_top(const std::vector<std::pair<int, std::string>>&, int n, std::ostream&);

void get_sort_data(std::vector<std::pair<int, std::string>>&,
                   const std::unordered_map<std::string, int>&,
                   Comparator&);


int main(int argc, char *argv[])
{
// [1] Parse comand line arguments
    int n {10};
    std::string in;
    std::string out;

    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " [-n NNN] in.txt out.txt" << std::endl;
        std::exit(1);
    }

    {
        int i = 1;

        if (std::string(argv[i]) == "-n")
        {
            std::stringstream convert(argv[++i]);
            if (!(convert >> n))
            {
                std::cerr << "Can't read n value" << std::endl;
                std::exit(1);
            }
            ++i;
        }

        in  = argv[i];
        out = argv[++i];
    }

// [2] Open input file
    std::ifstream ifs;
    ifs.open (in);

    if(!ifs)
    {
        std::cerr << "Can't open file with input data (" <<  in << ")" << std::endl;
        std::exit(1);
    }

// [3] Parse data
    std::string s;
    std::smatch m;
    std::regex regex {R"(http[s]?:\/\/([a-zA-Z0-9.-]+)(\/)?([\w\.\,\/\+]+)?)"};

    int urls {0};
    std::unordered_map<std::string, int> domain;
    std::unordered_map<std::string, int> path;

    while (ifs)
    {
        std::getline(ifs,s);

        while (std::regex_search(s,m,regex))
        {
            urls += 1;

            domain[m[1]] += 1;
            if (m[2].str() == "")
                path["/"] += 1;
            else
                path[m[2].str()+m[3].str()] += 1;

            s = m.suffix().str();
        }
    }

// [4] Close input file
    ifs.close();


// [5] Sort data
    Comparator comparator;

    std::vector<std::pair<int, std::string>> sort_domain;
    get_sort_data(sort_domain, domain, comparator);

    std::vector<std::pair<int, std::string>> sort_path;
    get_sort_data(sort_path, path, comparator);

// [6] Create output file
    std::ofstream ofs;
    ofs.open (out);

    if(!ofs)
    {
        std::cerr << "Can't create file to store outut data (out.txt)" << std::endl;
        std::exit(1);
    }

// [7] Write data to output file
    ofs << "total urls " << urls
        << ", domains "  << sort_domain.size()
        << ", paths "     << sort_path.size() << std::endl;

    ofs << std::endl;

    ofs << "top domains" << std::endl;
    print_top(sort_domain, n, ofs);

    ofs << std::endl;

    ofs << "top paths" << std::endl;
    print_top(sort_path, n, ofs);


// [8] Close output file
    ofs.close();

    return 0;
}

void print_top(const std::vector<std::pair<int, std::string>>& data,
               int n,
               std::ostream& ofs)
{
    int i = 0;

    for (auto x: data)
    {
        ofs << x.first << ' ' << x.second << std::endl;

        if (++i == n) return;
    }
}

void get_sort_data(std::vector<std::pair<int, std::string>>& data,
                   const std::unordered_map<std::string, int>& unordered_data, Comparator& comparator)
{
    for (auto i = unordered_data.begin(); i != unordered_data.end(); ++i)
        data.push_back(std::make_pair(i->second, i->first));

    std::sort(data.begin(), data.end(), comparator);
}


