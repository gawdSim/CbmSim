/*
 * File: commandline.cpp
 * Author: Sean Gallogly
 * Created on: 08/02/2022
 *
 * Description:
 *     This file implements the function prototypes in commandLine/commandline.h
 */

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

#include "commandline.h"
#include "logger.h"

// TODO: place in some global place, or convenience file, as there are
// duplicates across translation units now
const int NUM_CELL_TYPES = 8;
const int NUM_WEIGHTS_TYPES = 2;
const int NUM_SYN_CONS = 12;
const std::string CELL_IDS[NUM_CELL_TYPES] = {"MF", "GR", "GO", "BC",
                                              "SC", "PC", "IO", "NC"};
const std::string WEIGHTS_IDS[NUM_WEIGHTS_TYPES] = {"PFPC", "MFNC"};
const std::string SYN_CONS_IDS[NUM_SYN_CONS] = {"MFGR", "GRGO", "MFGO", "GOGO",
                                                "GOGR", "BCPC", "SCPC", "PCBC",
                                                "PCNC", "IOIO", "NCIO", "MFNC"};

/*
 * available commandline flags which take no argument
 */
const std::vector<std::string> command_line_single_opts{
    "--pfpc-off", "--mfnc-off", "--binary", "--cascade", "--stp", "--verbose",
};

/*
 * available commandline flags which take an argument. Each pair consists of the
 * short and long versions of the flag. Only one of either the short or long
 * versions of a flag may be specified in a single invocation of the cbm_sim
 * executable.
 *
 * e.g. how to obtain the first element of the array, and the first elem of the
 * pair: command_line_pair_opts[0].first == "-h"
 */

const std::vector<std::pair<std::string, std::string>> command_line_pair_opts{
    {"-h", "--help"},    // used when user wants usage information
    {"-v", "--visual"},  // used to specify whether to run in the terminal or a
                         // GUI interface
    {"-s", "--session"}, // used to specify the session file for running trials
    {"-i", "--input"},   // used to specify the file representing the initial
                         // state of a simulation for a run
    {"-o", "--output"}, // used to specify the file to output the final state of
                        // a simulation after a run
    {"-r", "--raster"}, // used to specify what cell types to collect raster
                        // information for during a run
    {"-p", "--psth"},   // used to specify what cell types to collect psth
                        // information for during a run
    {"-w", "--weights"}, // used to specify what synaptic weights to collect
                         // during a run
    {"-c", "--con-arrs"} // used to specify what synaptic connectivity arrays
                         // to collect
};

/*
 * Description:
 *     used to test whether the input string 'in_str' is a commandline flag or
 * not. currently supports only the paired flags above
 *
 *     TODO: integrate support for single flags above
 *
 */
static bool is_cmd_opt(std::string in_str) {
  for (auto opt : command_line_pair_opts) {
    if (in_str == opt.first || in_str == opt.second)
      return true;
  }
  return false;
}

/*
 * Description:
 *     searches for the string 'opt' in the buffer 'token_buf'
 */
static int cmd_opt_exists(std::vector<std::string> &token_buf,
                          std::string opt) {
  return (std::find(token_buf.begin(), token_buf.end(), opt) != token_buf.end())
             ? 1
             : 0;
}

/*
 * Description:
 *     searches for and returns the parameter associated with the flag/option
 * 'opt'. Returns the empty string if the flag des not exist in the input
 * buffer.
 */
static std::string get_opt_param(std::vector<std::string> &token_buf,
                                 std::string opt) {
  auto tp = std::find(token_buf.begin(), token_buf.end(), opt);
  if (tp != token_buf.end() && ++tp != token_buf.end()) {
    return std::string(*tp);
  }
  return "";
}

/*
 * Description:
 *     tests for whether the id 'id' is contained within the corresponding map.
 *
 * Implementation Notes:
 *     argument 'opt' is used to determine which array 'id' should be tested for
 *     presence within. Else the algorithm is rather straightforward
 *
 */
void test_for_valid_id(std::string &opt, std::string &id) {
  if (opt[1] == 'r' || opt[1] == 'p' || opt[2] == 'r' || opt[2] == 'p') {
    if (std::find(CELL_IDS, CELL_IDS + NUM_CELL_TYPES, id) ==
        CELL_IDS + NUM_CELL_TYPES) {
      LOG_FATAL("Invalid cell id '%s' found for option '%s'. Exiting...",
                id.c_str(), opt.c_str());
      exit(9);
    }
  } else if (opt[1] == 'w' || opt[2] == 'w') {
    if (std::find(WEIGHTS_IDS, WEIGHTS_IDS + NUM_WEIGHTS_TYPES, id) ==
        WEIGHTS_IDS + NUM_WEIGHTS_TYPES) {
      LOG_FATAL("Invalid weights id '%s' found for option '%s'. Exiting...",
                id.c_str(), opt.c_str());
      exit(10);
    }
  } else if (opt[1] == 'c' || opt[2] == 'c') {
    if (std::find(SYN_CONS_IDS, SYN_CONS_IDS + NUM_SYN_CONS, id) ==
        SYN_CONS_IDS + NUM_SYN_CONS) {
      LOG_FATAL(
          "Invalid connectivity id '%s' found for option '%s'. Exiting...",
          id.c_str(), opt.c_str());
      exit(11);
    }
  }
}

/*
 * Description:
 *     populates the input map, parsing out the parameter if needed.
 *
 * Implementation Notes:
 *     Three opts are expected: the raster cell ids, the psth cell ids, and the
 * synaptic weights ids. See the usage information from the output of 'cbm_sim
 * -h' for a list of the accepted cell-ids and synaptic weights ids. For any of
 * the three cases, 'param' is either a single id or a comma-separated list of
 * ids. Both cases are taken into account, with the latter case involving
 * parsing out the individual ids from the list.
 *
 *     Added argument validation 01/06/2023
 *
 */
static void fill_opt_map(std::map<std::string, bool> &opt_map, std::string opt,
                         std::string param) {
  if (param.empty()) {
    LOG_FATAL("No parameter given for option '%s'. Exiting...", opt.c_str());
    exit(12);
  }
  size_t prev_div = 0;
  size_t curr_div = param.find_first_of(',');
  std::string id;
  do {
    if (curr_div ==
        param.length() - 1) // test for placement of comma at end of string
    {
      LOG_FATAL("Invalid placement of comma for option '%s'. Exiting...",
                opt.c_str());
      exit(11);
    }
    if (curr_div - prev_div ==
        std::string::npos) // couldn't find ',', so test for single id
      id = param;
    else
      id = param.substr(prev_div, curr_div - prev_div);
    test_for_valid_id(opt, id);
    opt_map[id] = true; // creates this new entry
    prev_div = curr_div + 1;
    curr_div = param.find(',', curr_div + 1);
    if (curr_div == std::string::npos &&
        opt_map.find(param.substr(prev_div, curr_div - prev_div)) ==
            opt_map.end()) {
      id = param.substr(prev_div, curr_div - prev_div);
      test_for_valid_id(opt, id);
      opt_map[id] = true;
    }
  } while (curr_div != std::string::npos);
}

void print_usage_info() {
  std::cout << "Usage: ./cbm_sim [options]\n";
  std::cout << "Options:\n";
  std::cout << std::right << std::setw(10) << "\t--verbose"
            << "\t\tBe verbose about std output logs\n";
  std::cout << std::right << std::setw(10) << "\t-h, --help"
            << "\t\tprint this information and exit\n";
  std::cout << std::right << std::setw(20) << "\t-v, --visual [TUI|GUI]"
            << "\tspecify the visual mode the simulation is run in\n";
  std::cout << std::right << std::setw(20) << "\t-s, --session [FILE]"
            << "\tsets the simulation to run a session using FILE as the "
               "session file\n";
  std::cout << std::right << std::setw(20) << "\t-i, --input [FILE]"
            << "\tspecify the input simulation file\n";
  std::cout << std::right << std::setw(20) << "\t-o, --output [DIR]"
            << "\tspecify the output directory basename - all output is saved "
               "in 'ROOT/data/outputs'\n";
  std::cout << std::right << std::setw(10) << "\t--pfpc-off|--binary|--cascade"
            << "\tturns off or sets PFPC plasticity mode; options are mutually "
               "exclusive and work as follows:\n\n";
  std::cout << "\t\t\t\t \t--pfpc-off - turns PFPC plasticity off\n";
  std::cout << "\t\t\t\t \t--binary - turns PFPC plasticity on and sets the "
               "type of plasticity to 'binary'\n";
  std::cout << "\t\t\t\t \t--cascade - turns PFPC plasticity on and sets the "
               "type of plasticity to 'cascade'\n\n";
  std::cout << std::right << std::setw(10) << "\t"
            << "\t\t\tif none of these options is given, PFPC plasticity is "
               "turned on and set to 'graded' by default\n\n";
  std::cout << std::right << std::setw(10) << "\t--mfnc-off"
            << "\t\tturns off MFNC plasticity; same as default for now (ie is "
               "not tuned)\n";
  std::cout << "\t-c, --con-arrs {[CODE]} comma-separated list of synapse "
               "codes indicating what connectivity arrays are saved.\n";
  std::cout << "\t                        Saves both pre and post arrays "
               "whenever possible. CODEs are:\n\n";
  std::cout << "\t\t\t\t \tMFGR - Mossy Fiber to Granule Cells\n";
  std::cout << "\t\t\t\t \tGRGO - Granule Cells to Golgi Cells\n";
  std::cout << "\t\t\t\t \tMFGO - Mossy Fiber to Granule Cells\n";
  std::cout << "\t\t\t\t \tGOGO - Golgi Cells to Golgi Cells\n";
  std::cout << "\t\t\t\t \tGOGR - Golgi Cells to Granule Cells\n";
  std::cout << "\t\t\t\t \tBCPC - Basket Cells to Purkinje Cells\n";
  std::cout << "\t\t\t\t \tSCPC - Stellate Cells to Purkinje Cells\n";
  std::cout << "\t\t\t\t \tPCBC - Purkinje Cells to Basket Cells\n";
  std::cout << "\t\t\t\t \tPCNC - Purkinje Cells to Nucleus Cells\n";
  std::cout << "\t\t\t\t \tIOIO - Inferior Olives to Inferior Olives\n";
  std::cout << "\t\t\t\t \tNCIO - Nucleus Cells to Inferior Olives\n";
  std::cout << "\t\t\t\t \tMFNC - Mossy Fiber to Nucleus Cells\n\n";
  std::cout << "\t-r, --raster {[CODE]} comma-separated list of cell ids to be "
               "saved for that cell type. Possible CODEs are:\n\n";
  std::cout << "\t\t\t\t \tMF - Mossy Fiber\n";
  std::cout << "\t\t\t\t \tGR - Granule Cell\n";
  std::cout << "\t\t\t\t \tGO - Golgi Cell\n";
  std::cout << "\t\t\t\t \tSC - Stellate Cell\n";
  std::cout << "\t\t\t\t \tBC - Basket Cell\n";
  std::cout << "\t\t\t\t \tPC - Purkinje Cell\n";
  std::cout << "\t\t\t\t \tNC - Deep Nucleus Cell\n";
  std::cout << "\t\t\t\t \tIO - Inferior Olive Cell\n\n";
  std::cout << "\t-p, --psth {[CODE]} comma-separated list of cell ids to be "
               "saved for that cell type. Possible CODEs are identical with "
               "those for rasters.\n\n";
  std::cout << "\t-w, --weights {[CODE]} comma-separated list of weights ids "
               "to be saved. Possible CODEs are:\n\n";
  std::cout << "\t\t\t\t  \tPFPC - parallel-fiber to purkinje synapse\n";
  std::cout << "\t\t\t\t  \tMFNC - mossy-fiber to deep nucleus synapse\n\n";
  std::cout << "Output filenames are automatically generated: given an -o "
               "option of 'BASENAME', the relative filepaths from project ROOT "
               "are as follows:\n\n";
  std::cout << "\t'ROOT/data/outputs/BASENAME/BASENAME.[EXTENSION]\n\n";
  std::cout << "Here [EXTENSION] is expressed as the cell/synapse CODE plus "
               "the symbols ";
  std::cout
      << "'r', 'p' and 'w' for 'raster,' 'psth', and 'weight' data.\n"
         "For example, 'basename.pcr' is a raster file for pc cells while ";
  std::cout << "basename.pfpcw is a weight file of the pf to pc weights\n\n";
  std::cout << "Example usage:\n\n";
  std::cout << "1) constructs a bunny, which is "
               "saved to file 'ROOT/data/outputs/bunny/bunny.sim':\n\n";
  std::cout << "\t./cbm_sim -o bunny\n\n";
  std::cout << "1) constructs a bunny, which is "
               "saved to file 'ROOT/data/outputs/bunny/bunny.sim' and";
  std::cout << "saves GRGO pre-synaptic and post-synaptic arrays to file:\n\n";
  std::cout << "\t./cbm_sim -o bunny -c GRGO\n\n";
  std::cout << "2) uses file 'acquisition.json' to train the input simulation "
               "'bunny.sim' with PFPC plasticity on and set to graded "
               "and MFNC plasticity off:\n\n";
  std::cout << "\t./cbm_sim -s acquisition.json -i bunny.sim -o "
               "acquisition --mfnc-off\n\n";
  std::cout << "3) uses file 'acquisition.json' to train the input simulation "
               "'bunny.sim' with PFPC and MFNC plasticity on and set "
               "to graded;\n";
  std::cout << "   saves purkinje cell, stellate cell, and basket cell rasters "
               "to files located in 'ROOT/data/outputs/acqusition'\n\n";
  std::cout << "\t./cbm_sim -s acquisition.json -i bunny.sim -o "
               "acquisition -r PC,SC,BC\n\n";
}

/*
 * Implementation Notes:
 *     There are three main computational components to this function:
 *
 *     1) collecting the commandline tokens, as-is, into a vector of strings
 * (for easier manipulation) 2) looping over the single options and checking for
 * the presence of each in-turn 3) looping over the pair options and checking
 * for the presence of each in-turn
 *
 *     the second and third components assign the relevant attributes of the
 * parsed_commandline reference. the third component checks whether the user
 * entered both the short and long version of a given option pair, and exits
 * with a fatal log if so.
 */
void parse_commandline(int *argc, char ***argv, parsed_commandline &p_cl) {
  std::vector<std::string> tokens;
  // get command line args in a vector for easier parsing.
  for (char **iter = *argv; iter != *argv + *argc; iter++) {
    tokens.push_back(std::string(*iter));
  }

  // fill plasticity values, testing for mutually exclusive options
  bool pfpc_opt_found = false;
  for (auto single_opt : command_line_single_opts) {
    if (cmd_opt_exists(tokens, single_opt)) {
      if (single_opt.find("mfnc") != std::string::npos) {
        p_cl.mfnc_plasticity = single_opt.substr(7, std::string::npos);
      } else if (single_opt.find("verbose") != std::string::npos) {
        p_cl.verbose = single_opt.substr(2, std::string::npos);
      } else if (single_opt.find("stp") != std::string::npos) {
        p_cl.stp = "on";
      } else if (!pfpc_opt_found) {
        int offset = (single_opt.find("pfpc") != std::string::npos) ? 7 : 2;
        p_cl.pfpc_plasticity = single_opt.substr(offset, std::string::npos);
        pfpc_opt_found = true;
      } else {
        LOG_FATAL("Mutually exclusive or duplicate pfpc plasticity arguments "
                  "found. Exiting...");
        exit(1);
      }
    }
  }

  // first pass: get options and associated opt_params.
  for (auto opt : command_line_pair_opts) {
    int first_opt_exist = cmd_opt_exists(tokens, opt.first);
    int second_opt_exist = cmd_opt_exists(tokens, opt.second);
    int opt_sum = first_opt_exist + second_opt_exist;
    std::string this_opt, this_param;
    char opt_char_code;
    auto curr_token_iter = tokens.begin();
    p_cl.cmd_name = *curr_token_iter;
    switch (opt_sum) {
    case 2:
      LOG_FATAL("Found both short and long form of option '%s'. Exiting...",
                opt.first.c_str());
      exit(1);
      break;
    case 1:
      this_opt = (first_opt_exist == 1) ? opt.first : opt.second;
      // both give the same thing, it is a matter of which exists, the
      // long or the short version.
      opt_char_code = (first_opt_exist == 1) ? this_opt[1] : this_opt[2];
      if (opt_char_code == 'h')
        p_cl.print_help = "help";
      else {
        this_param = get_opt_param(tokens, this_opt);
        curr_token_iter = std::find(tokens.begin(), tokens.end(), this_param);
      }
      switch (opt_char_code) {
      case 'v':
        p_cl.vis_mode = this_param;
        break;
      case 's':
        p_cl.session_file = this_param;
        break;
      case 'i':
        p_cl.input_sim_file = this_param;
        break;
      case 'o':
        p_cl.output_basename = this_param;
        break;
      case 'r':
        fill_opt_map(p_cl.raster_files, this_opt, this_param);
        break;
      case 'p':
        fill_opt_map(p_cl.psth_files, this_opt, this_param);
        break;
      case 'w':
        fill_opt_map(p_cl.weights_files, this_opt, this_param);
        break;
      case 'c':
        fill_opt_map(p_cl.conn_arrs_files, this_opt, this_param);
      }
      break;
    case 0:
      continue;
      break;
    }
  }
}

// I am sorry in advance for this implementation. C++ doesn't have reflection. A
// shame.
bool p_cmdline_is_empty(parsed_commandline &p_cl) {
  return p_cl.print_help.empty() && p_cl.verbose.empty() && p_cl.stp.empty() &&
         p_cl.vis_mode.empty() && p_cl.session_file.empty() &&
         p_cl.input_sim_file.empty() && p_cl.output_basename.empty() &&
         p_cl.pfpc_plasticity.empty() && p_cl.mfnc_plasticity.empty() &&
         p_cl.raster_files.empty() && p_cl.psth_files.empty() &&
         p_cl.weights_files.empty() && p_cl.conn_arrs_files.empty();
}

/*
 * Implementation Notes:
 *
 * This function validates the parsed_commandline, first for build mode and
 * second for run mode. Admittedly, this function is a bit of a mess, as not
 * every accidental case has been tested for, but the correct input for each
 * mode is verified, and *most* error states are caught. Further testing is most
 * definitely required.
 *
 */

void validate_commandline(parsed_commandline &p_cl) {
  if (p_cmdline_is_empty(p_cl)) // only executable given, open the gui
  {
    p_cl.vis_mode = "GUI";
    p_cl.pfpc_plasticity = "graded";
    p_cl.mfnc_plasticity = "off";
  } else {
    /* for now, print usage info regardless of other arguments.
     * in future, if there is a commandline error, print usage info then exit
     */
    if (!p_cl.print_help.empty()) {
      print_usage_info();
      exit(0);
    }
    if (!p_cl.verbose.empty()) {
      logger_setLevel(LogLevel_DEBUG);
    }
    if (!p_cl.session_file.empty()) // checking validity of input for run mode
    {
      if (!p_cl.input_sim_file.empty()) {
        std::string input_sim_file_fullpath;
        // verify whether the input simulation file can be found recursively
        // from {PROJECT_ROOT}data/outputs/
        if (!file_exists(OUTPUT_DATA_PATH, p_cl.input_sim_file,
                         input_sim_file_fullpath)) {
          LOG_FATAL("Could not find input simulation file '%s'. Exiting...",
                    p_cl.input_sim_file.c_str());
          exit(11);
        }
        p_cl.input_sim_file = input_sim_file_fullpath;
      } else {
        LOG_FATAL("no input simulation specified in run mode. exiting...");
        exit(8);
      }
      if (p_cl.output_basename.empty()) {
        LOG_FATAL("You must specify an output basename. Exiting...");
        exit(7);
      }
      if (p_cl.pfpc_plasticity.empty()) {
        LOG_DEBUG("Turning PFPC plasticity on to default mode 'graded'...");
        p_cl.pfpc_plasticity = "graded";
      } else {
        // just notify user what we already set above
        if (p_cl.pfpc_plasticity == "binary")
          LOG_DEBUG("Turning PFPC plasticity on in 'binary' mode...");
        else if (p_cl.pfpc_plasticity == "cascade")
          LOG_DEBUG("Turning PFPC plasticity on in 'cascade' mode...");
        else if (p_cl.pfpc_plasticity == "off")
          LOG_DEBUG("Turning PFPC plasticity off..");
      }
      if (p_cl.mfnc_plasticity.empty()) {
        LOG_DEBUG("Turning MFNC plasticity to default mode 'off'...");
        p_cl.mfnc_plasticity = "off";
      } else if (p_cl.mfnc_plasticity == "off") {
        LOG_DEBUG("Turning MFNC plasticity off...");
      } else if (p_cl.mfnc_plasticity == "graded") {
        LOG_DEBUG("Turning MFNC plasticity on in 'graded' mode...");
        p_cl.mfnc_plasticity = "graded";
      }
      if (p_cl.vis_mode.empty()) {
        LOG_DEBUG("Visual mode not specified in run mode. Setting to default "
                  "value of 'TUI'...");
        p_cl.vis_mode = "TUI";
      }
      std::string input_sess_file_fullpath;
      if (!file_exists(INPUT_DATA_PATH, p_cl.session_file,
                       input_sess_file_fullpath)) {
        LOG_FATAL("Could not find input session file '%s'. Exiting...",
                  p_cl.session_file.c_str());
        exit(11);
      }
      p_cl.session_file = input_sess_file_fullpath;
    } else if (!p_cl.input_sim_file.empty()) {
      if (p_cl.vis_mode.empty()) {
        LOG_DEBUG(
            "Visual mode not specified. Setting to default value of 'TUI'...");
        p_cl.vis_mode = "TUI";
      } else if (p_cl.vis_mode == "GUI") {
        LOG_FATAL("Cannot specify visual mode 'GUI' in connectivity collect "
                  "mode. Exiting...");
        exit(7);
      }
      if (p_cl.conn_arrs_files.empty()) {
        LOG_FATAL("You must specify at least one connectivity array to save. "
                  "Exiting...");
        exit(8);
      }
      if (p_cl.output_basename.empty()) {
        LOG_FATAL("You must specify an output basename. Exiting...");
        exit(9);
      }
      std::string input_sim_file_fullpath;
      // verify whether the input simulation file can be found recursively
      // from {PROJECT_ROOT}data/outputs/
      if (!file_exists(OUTPUT_DATA_PATH, p_cl.input_sim_file,
                       input_sim_file_fullpath)) {
        LOG_FATAL("Could not find input simulation file '%s'. Exiting...",
                  p_cl.input_sim_file.c_str());
        exit(11);
      }
      p_cl.input_sim_file = input_sim_file_fullpath;
    } else {
      if (p_cl.output_basename.empty()) {
        LOG_FATAL(
            "You must specify an output basename in build mode. Exiting...");
        exit(9);
      }
      if (p_cl.vis_mode.empty()) {
        LOG_DEBUG("Visual mode not specified in run mode. Setting to default "
                  "value of 'TUI'...");
        p_cl.vis_mode = "TUI";
      }
    }
  }
}

// NOTE: if you want to make this guy available outside this translation unit,
// remove the static keyword
void parse_and_validate_parsed_commandline(int *argc, char ***argv,
                                           parsed_commandline &p_cl) {
  parse_commandline(argc, argv, p_cl);
  validate_commandline(p_cl);
}

// std::string overloads the assignment operator such that all string data is
// copied, ensuring a deep copy.
void cp_parsed_commandline(parsed_commandline &from_p_cl,
                           parsed_commandline &to_p_cl) {
  to_p_cl.cmd_name = from_p_cl.cmd_name;
  to_p_cl.print_help = from_p_cl.print_help;
  to_p_cl.verbose = from_p_cl.verbose;
  to_p_cl.stp = from_p_cl.stp;
  to_p_cl.vis_mode = from_p_cl.vis_mode;
  to_p_cl.session_file = from_p_cl.session_file;
  to_p_cl.input_sim_file = from_p_cl.input_sim_file;
  to_p_cl.output_basename = from_p_cl.output_basename;
  to_p_cl.pfpc_plasticity = from_p_cl.pfpc_plasticity;
  to_p_cl.mfnc_plasticity = from_p_cl.mfnc_plasticity;

  to_p_cl.raster_files = from_p_cl.raster_files;
  to_p_cl.psth_files = from_p_cl.psth_files;
  to_p_cl.weights_files = from_p_cl.weights_files;
  to_p_cl.conn_arrs_files = from_p_cl.conn_arrs_files;
}

std::string parsed_commandline_to_str(parsed_commandline &p_cl) {
  std::stringstream p_cl_buf;
  p_cl_buf << "{ 'cmd_name', '" << p_cl.cmd_name << "' }\n";
  p_cl_buf << "{ 'print_help', '" << p_cl.print_help << "' }\n";
  p_cl_buf << "{ 'verbose', '" << p_cl.verbose << "' }\n";
  p_cl_buf << "{ 'stp', '" << p_cl.stp << "' }\n";
  p_cl_buf << "{ 'vis_mode', '" << p_cl.vis_mode << "' }\n";
  p_cl_buf << "{ 'session_file', '" << p_cl.session_file << "' }\n";
  p_cl_buf << "{ 'input_sim_file', '" << p_cl.input_sim_file << "' }\n";
  p_cl_buf << "{ 'output_basename', '" << p_cl.output_basename << "' }\n";
  p_cl_buf << "{ 'pfpc_plasticity', '" << p_cl.pfpc_plasticity << "' }\n";
  p_cl_buf << "{ 'mfnc_plasticity', '" << p_cl.mfnc_plasticity << "' }\n";
  p_cl_buf << "{ 'raster_files' :\n";
  for (auto pair : p_cl.raster_files) {
    p_cl_buf << "{ '" << pair.first << "', '" << pair.second << "' }\n";
  }
  p_cl_buf << "}\n";
  p_cl_buf << "{ 'psth_files' :\n";
  for (auto pair : p_cl.psth_files) {
    p_cl_buf << "{ '" << pair.first << "', '" << pair.second << "' }\n";
  }
  p_cl_buf << "}\n";
  p_cl_buf << "{ 'weights_files' :\n";
  for (auto pair : p_cl.weights_files) {
    p_cl_buf << "{ '" << pair.first << "', '" << pair.second << "' }\n";
  }
  p_cl_buf << "}\n";
  p_cl_buf << "{ 'conn_arrs_files' :\n";
  for (auto pair : p_cl.conn_arrs_files) {
    p_cl_buf << "{ '" << pair.first << "', '" << pair.second << "' }\n";
  }
  p_cl_buf << "}\n";
  return p_cl_buf.str();
}

std::ostream &operator<<(std::ostream &os, parsed_commandline &p_cl) {
  return os << parsed_commandline_to_str(p_cl);
}
