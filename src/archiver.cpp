#include "archiver.h"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>

template <class Archive>
void save(Archive &ar, QPoint const &p)
{
  ar(cereal::make_nvp("x", p.x()), cereal::make_nvp("y", p.y()));
}

template <class Archive>
void load(Archive &ar, QPoint &p)
{
  ar(cereal::make_nvp("x", p.rx()), cereal::make_nvp("y", p.ry()));
}

template <class Archive>
void save(Archive &ar, const board_piece &bp)
{
  ar(cereal::make_nvp("label", bp.label()),
     cereal::make_nvp("type", bp.type()),
     cereal::make_nvp("orientation", bp.ori()),
     cereal::make_nvp("position", bp.pos()));
}

template <class Archive>
void load(Archive &ar, board_piece &bp)
{
  std::string label;
  board_piece_t type;
  orientation_t ori;
  QPoint pos;

  ar(cereal::make_nvp("label", label),
     cereal::make_nvp("type", type),
     cereal::make_nvp("orientation", ori),
     cereal::make_nvp("position", pos));

  bp.set_label(label);
  bp.set_type(type);
  bp.set_ori(ori);
  bp.set_pos(pos);
}

namespace board_archiver {

  void save(const std::vector<std::unique_ptr<board_piece>> &board_pieces,
            const std::string &filename)
  {
    std::ofstream save_file(filename);
    {
      cereal::JSONOutputArchive archive(save_file);
      archive(cereal::make_nvp("board_pieces", board_pieces));
    }
    save_file.close();
  }

  void load(std::vector<std::unique_ptr<board_piece>> &board_pieces,
            const std::string &filename)
  {
    std::ifstream save_file(filename);
    if (save_file.good()) {
      cereal::JSONInputArchive archive(save_file);
      archive(cereal::make_nvp("board_pieces", board_pieces));
    }
    save_file.close();
  }

}
