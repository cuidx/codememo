//gcc test.cpp -o test1 -std=c++11 -lstdc++  不加-lstdc++ 编译不过
//g++ test.cpp -o test -std=c++11   不用加-lstdc++ ，g++默认带了

#include <iostream>
#include <string>

using namespace std;

string getMaxUniqueSubString(const string& str) {
	int len = str.length();
	if (len <= 1) return str;
	int left = 0, right = 0;
	int maxLen = 1;
	int dic[128] = { 0 };
	string ans;
	while (right < len) {
		auto& c = str[right];
		if (dic[c] == 0) {
			dic[c]++;
		} else {
			int n = right - left;
			if (n > maxLen) {
				maxLen = n;
				ans = str.substr(left,n);
			}
			while (left < right && str[left] != str[right]) {
				dic[str[left]] = 0;
				left++;
			}
			left++;
		}
		right++;
	}
	int n = right - left;
	if (n > maxLen) {
		maxLen = n;
		ans = str.substr(left,n);
	}
	return ans;
}

int main() {
	string s = "abfgssdfg";
	string ans = getMaxUniqueSubString(s);
	cout << ans << endl;
	return 0;
}