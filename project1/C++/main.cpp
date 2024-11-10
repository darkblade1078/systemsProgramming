#include "BST.hpp"
#include <string>
using std::string;

int main()
{

  BST store;
  string word;

  std::cout << "Please enter a series of words to store in a BST" << std::endl;
  std::cout << "Type 'STOP' to end the series and output the BST" << std::endl;
  std::cout << "In postorder" << std::endl;


  while (1)
    {
      std::cout << "Word = ";
      std::cin >> word;
      if (word == "STOP") break;
      store.insert(word);
    }

  std::cout << "The BST ouput in postorder is "<< std::endl;
  store.postOrder(std::cout);
  std::cout << std::endl;



  return 1;
}
