#include <cstdlib>
#include <cstdio>
#include "defs.h"

/*
  Copyright (c) 2003-2015 Tommi Junttila
  Released under the GNU Lesser General Public License version 3.

  This file is part of bliss.

  bliss is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, version 3 of the License.

  bliss is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with bliss.  If not, see <http://www.gnu.org/licenses/>.
*/

// Silvan Sievers
#include <iostream>

namespace bliss {

void
fatal_error(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr,"Bliss fatal error: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\nAborting!\n");
  va_end(ap);
  exit(1);
}

void _OUT_OF_MEMORY(const char* file, int line) {
    std::cerr << file << ":" << line << " out of memory" << std::endl;
    throw BlissMemoryOut();
}

}
