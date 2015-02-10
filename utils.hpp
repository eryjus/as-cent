
//-----------------------------------------------------------------------------------------------
// Finally, some worker function primarily used in the parser, but could be useful somewhere
// else.
//-----------------------------------------------------------------------------------------------

inline std::string rtrim(const std::string& s, const std::string& delimiters = " \f\n\r\t\v")
{
    return s.substr(0, s.find_last_not_of(delimiters) + 1);
}

//-----------------------------------------------------------------------------------------------

inline std::string ltrim(const std::string& s, const std::string& delimiters = " \f\n\r\t\v")
{
    return s.substr(s.find_first_not_of(delimiters));
}

//-----------------------------------------------------------------------------------------------

inline std::string trim(const std::string& s, const std::string& delimiters = " \f\n\r\t\v")
{
    return ltrim(rtrim(s, delimiters), delimiters);
}
