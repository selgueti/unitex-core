/**
 * Unitex
 *
 * Copyright (C) 2001-2021 Université Paris-Est Marne-la-Vallée <unitex@univ-mlv.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 *
 */

#ifndef MULTI2DELAFH
#define MULTI2DELAFH

#ifndef HAS_UNITEX_NAMESPACE
#define HAS_UNITEX_NAMESPACE 1
#endif

#include <memory>
#include <vector>

#include "Pattern.h"
#include "Unicode.h"

namespace unitex {

/**
 * Describes a command in the file that configures the parser for multidelaf strings.
 * A multidelaf string is an output of Explore graph paths made of a sequence of 
 * Delaf tags.
 */
class ConfigCommand {
 public:
  static constexpr const char* const COMMA_COPY  = ",copy";
  static constexpr const char* const DOT_COPY    = ".copy";
  static constexpr const char* const PLUS_COPY   = "+copy";
  static constexpr const char* const COLUMN_COPY = ":copy";

  static std::unique_ptr<ConfigCommand> tokenize_config_command(
      unichar* str, const char* config_filename);

  ConfigCommand(unichar* lemma, unichar* part_of_speech,
                struct list_ustring* semantic_codes,
                struct list_ustring* inflectional_codes);
  // Not copyable or movable
  ConfigCommand(const ConfigCommand&)                  = delete;
  ConfigCommand(ConfigCommand&&)                       = delete;
  ConfigCommand& operator=(const ConfigCommand& other) = delete;
  ConfigCommand& operator=(ConfigCommand&& other)      = delete;
  ~ConfigCommand();
  unichar* get_lemma() const;
  unichar* get_part_of_speech() const;

 private:
  static unichar* tokenize_lemma(unichar** ptr, const char* config_filename);
  static unichar* tokenize_part_of_speech(unichar** ptr,
                                          const char* config_filename);
  static unichar* tokenize_one_semantic_code(unichar** ptr,
                                             const char* config_filename);
  static struct list_ustring* tokenize_semantic_codes(
      unichar** ptr, const char* config_filename);
  static unichar* tokenize_one_inflectional_code(unichar** ptr,
                                                 const char* config_filename);
  static struct list_ustring* tokenize_inflectional_codes(
      unichar* str, const char* config_filename);
  unichar* _lemma;
  unichar* _part_of_speech;
  struct list_ustring* _semantic_codes;
  struct list_ustring* _inflectional_codes;
};

/**
 * Describes a line in the file that configures the parser for multidelaf strings.
 * A multidelaf string is an output of Explore graph paths made of a sequence of 
 * Delaf tags.
 */
class ConfigLine {
 public:
  static constexpr int NOT_SPECIFIED = -1;  // nb_required_tag default value

  static std::unique_ptr<ConfigLine> tokenize_config_line(
      unichar* line, const char* config_filename);

  ConfigLine(struct pattern* pattern, int nb_required_tag,
             std::shared_ptr<ConfigCommand> _config_command);
  // Not copyable or movable
  ConfigLine(const ConfigLine&)                  = delete;
  ConfigLine(ConfigLine&&)                       = delete;
  ConfigLine& operator=(const ConfigLine& other) = delete;
  ConfigLine& operator=(ConfigLine&& other)      = delete;
  ~ConfigLine();
  static int advance_to_next_no_blank_char(unichar** str);

 private:
  static int recognize_pattern_token(unichar** ptr, unichar* res);
  static int tokenize_nb_required_tag(unichar** ptr, int* res);
  struct pattern* _pattern;  // lexical mask
  // if _nb_required_tag equals to NOT_SPECIFIED, the number of delaf tags that must match the pattern is not specified
  // else if _nb_required_tag equals to 0, the pattern must not match any delaf tag
  // otherwise the numer of delaf tag that must match the pattern
  const int _nb_required_tag;
  const std::shared_ptr<ConfigCommand> _config_command;
};

/**
 * Multi2Delaf class
 */
class Multi2Delaf {
 public:
  Multi2Delaf(const char* config_filename);
  // Not copyable or movable
  Multi2Delaf(const Multi2Delaf&)                  = delete;
  Multi2Delaf(Multi2Delaf&&)                       = delete;
  Multi2Delaf& operator=(const Multi2Delaf& other) = delete;
  Multi2Delaf& operator=(Multi2Delaf&& other)      = delete;
  void parse_config_file();

 private:
  static int read_line_config_file(U_FILE* config_file, unichar* buffer,
                                   int size_buffer);
  void load_config_file(U_FILE* config_file);
  const VersatileEncodingConfig _vec = VEC_DEFAULT;
  std::vector<std::shared_ptr<ConfigLine>> _config_lines;
  const char* _config_filename;
};

}  // namespace unitex

#endif
