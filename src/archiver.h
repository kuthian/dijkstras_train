#ifndef BOARD_SAVER_H
#define BOARD_SAVER_H

#include <memory>
#include <vector>

#include "board_piece.h"

namespace board_archiver {

	void save(const std::vector<std::unique_ptr<board_piece>>&,
	          const std::string& filename);

	void load(std::vector<std::unique_ptr<board_piece>>&,
	          const std::string& filename);

}

#endif