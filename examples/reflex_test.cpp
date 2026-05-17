//
// Created by Terry on 5/17/2026.
//

#include <iostream>
#include <string>

#include <reflex/matcher.h>

int main() {
  std::string text = "Contact us at info@example.com for help.";

  // Define a pattern for a simple email address
  // Note: Use reflex::Matcher::convert to support Unicode if needed
  reflex::Pattern pattern("\\w+@\\w+\\.\\w+");
  std::string s_pattern = "\\w+@\\w+\\.\\w+";

  // Create a matcher for the text
  reflex::Matcher matcher(s_pattern, text);

  // Find all matches in the text
  while (matcher.find() != 0) {
    std::cout << "Found email: " << matcher.text() << " at position " << matcher.first() << std::endl;
  }

  return 0;
}
