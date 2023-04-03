#ifndef BOARD_SAVER_H
#define BOARD_SAVER_H

#include <memory>
#include <vector>

#include "board_piece.h"

namespace board_archiver {

	const std::string default_savefile = "board_state.sav";

	void save(const std::vector<std::unique_ptr<board_piece>>&,
	          const std::string& filename = default_savefile);

	void load(std::vector<std::unique_ptr<board_piece>>&,
	          const std::string& filename = default_savefile);

}

#endif