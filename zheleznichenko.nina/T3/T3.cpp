#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>

struct Point {
  int x, y;
};

struct Polygon {
  std::vector<Point> points;
};

void skipSpaces(std::istream& is) {
  while (is.peek() == ' ' || is.peek() == '\t') {
    is.ignore();
  }
}

Point readPoint(std::istream& is) {
  char open = 0, semi = 0, close = 0;
  int x = 0, y = 0;

  skipSpaces(is);
  if (!(is >> open) || open != '(') { is.setstate(std::ios::failbit); return { 0,0 }; }
  if (!(is >> x)) { is.setstate(std::ios::failbit); return { 0,0 }; }
  skipSpaces(is);
  if (!(is >> semi) || semi != ';') { is.setstate(std::ios::failbit); return { 0,0 }; }
  if (!(is >> y)) { is.setstate(std::ios::failbit); return { 0,0 }; }
  skipSpaces(is);
  if (!(is >> close) || close != ')') { is.setstate(std::ios::failbit); return { 0,0 }; }

  return Point{ x, y };
}

bool equalPoints(const Point& a, const Point& b) {
  return a.x == b.x && a.y == b.y;
}

bool pointLess(const Point& a, const Point& b) {
  return (a.x < b.x) || (a.x == b.x && a.y < b.y);
}

bool isPerm(const Polygon& a, const Polygon& b) {
  if (a.points.size() != b.points.size()) return false;
  std::vector<Point> pa = a.points, pb = b.points;
  std::sort(pa.begin(), pa.end(), pointLess);
  std::sort(pb.begin(), pb.end(), pointLess);
  return std::equal(pa.begin(), pa.end(), pb.begin(), equalPoints);
}

int dotProduct(Point a, Point b, Point c) {
  return (a.x - b.x) * (c.x - b.x) + (a.y - b.y) * (c.y - b.y);
}

bool isRightTriangle(const Polygon& p) {
  if (p.points.size() != 3) return false;
  return dotProduct(p.points[0], p.points[1], p.points[2]) == 0 ||
    dotProduct(p.points[1], p.points[0], p.points[2]) == 0 ||
    dotProduct(p.points[0], p.points[2], p.points[1]) == 0;
}

Polygon parsePolygon(const std::string& line) {
  if (line.empty()) throw std::runtime_error("empty");
  std::istringstream iss(line);
  int n = 0;
  if (!(iss >> n) || n < 3) throw std::runtime_error("bad n");

  Polygon poly;
  std::generate_n(std::back_inserter(poly.points), n, std::bind(readPoint, std::ref(iss)));

  if (iss.fail()) throw std::runtime_error("bad format");

  std::string extra;
  if (iss >> extra) throw std::runtime_error("extra data");

  return poly;
}

int main(int argc, char* argv[]) {
  if (argc < 2) return 1;
  std::ifstream file(argv[1]);
  if (!file) return 1;

  std::vector<Polygon> data;
  std::string line;
  while (std::getline(file, line)) {
    try {
      if (!line.empty()) {
        data.push_back(parsePolygon(line));
      }
    }
    catch (...) {
    }
  }

  std::string cmd;
  while (std::cin >> cmd) {
    try {
      if (cmd == "PERMS") {
        std::string rest;
        std::getline(std::cin, rest);
        Polygon target = parsePolygon(rest);
        std::cout << std::count_if(data.begin(), data.end(),
          std::bind(isPerm, std::placeholders::_1, target)) << "\n";
      }
      else if (cmd == "RIGHTSHAPES") {
        std::cout << std::count_if(data.begin(), data.end(), isRightTriangle) << "\n";
      }
      else {
        std::cout << "<INVALID COMMAND>\n";
        std::cin.ignore(10000, '\n');
      }
    }
    catch (...) {
      std::cout << "<INVALID COMMAND>\n";
    }
  }
  return 0;
}
