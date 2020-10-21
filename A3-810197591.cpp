#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map> 
#include <tuple>
#include <bits/stdc++.h> 
#include <algorithm>
#include <sstream>

using namespace std;

#define CSV_path 1
#define NUM_OF_HEADINGS 7
#define DAYS_IN_WEEK 7
#define SEPERATE ','
#define NEXT_WORD_INDEX 0
#define WORD 1

enum day {Saturday = 0, Sunday, Monday, Tuesday, Wednesday, Thursday, Friday};
enum csv_headings {MovieName = 0, CinemaName, Day, StartingTime, FinishingTime, Price, Hall};
enum command_mode {GetAllMovies, GetSchedule, Invalid};
enum command_placeHolder {Get = 0, Arg1, Arg2};
enum time_placeHolder {Hour = 0, Minute};

typedef tuple<int, int> Time;

struct Movie {
  string CinemaName;
  string MovieName;
  day Day;
  Time StartingTime;
  Time FinishingTime;
  int Price;
  int Hall;
};

typedef vector<Movie> MovieList;
typedef vector<MovieList> WeekShowTimes;
typedef vector<string> ConsoleMap;

bool first_time_is_earlier(Time time_1, Time time_2)
{
  return get<Hour>(time_1) < get<Hour>(time_2) ||
    (get<Hour>(time_1) == get<Hour>(time_2) && get<Minute>(time_1) <= get<Minute>(time_2));  
}

bool compare_by_StartTime(const Movie& movie_1, const Movie &movie_2)
{
  return first_time_is_earlier(movie_1.StartingTime, movie_2.StartingTime);
}

bool compare_by_Price(const Movie& movie_1, const Movie &movie_2)
{
  return movie_1.Price < movie_2.Price;
}

bool compare_by_MovieName(const Movie& movie_1, const Movie &movie_2)
{
  return movie_1.MovieName < movie_2.MovieName;
}

bool compare_by_CinemaName(const Movie& movie_1, const Movie &movie_2)
{
  return movie_1.CinemaName < movie_2.CinemaName;
}

bool compare_by_Day(const Movie& movie_1, const Movie &movie_2)
{
  return movie_1.Day < movie_2.Day;
}

MovieList sort_by_StartTime(MovieList movie_list)
{
  sort(movie_list.begin(), movie_list.end(), compare_by_StartTime);
  return movie_list;
}

MovieList sort_by_Price(MovieList movie_list)
{
  sort(movie_list.begin(), movie_list.end(), compare_by_Price);
  return movie_list;
}

MovieList sort_by_MovieName(MovieList movie_list)
{
  sort(movie_list.begin(), movie_list.end(), compare_by_MovieName);
  return movie_list;
}

MovieList sort_by_CinemaName(MovieList movie_list)
{
  sort(movie_list.begin(), movie_list.end(), compare_by_CinemaName);
  return movie_list;
}

MovieList sort_by_Day(MovieList movie_list)
{
  sort(movie_list.begin(), movie_list.end(), compare_by_Day);
  return movie_list;
}

bool times_are_same(Time time_1, Time time_2)
{
  return (get<Hour>(time_1) == get<Hour>(time_2)
          && get<Minute>(time_1) == get<Minute>(time_2));
}

string convert_day2str(day day_num)
{
  unordered_map<day, string> day_str_map =
    {{Saturday, "Saturday"}, {Sunday, "Sunday"}, {Monday, "Monday"}, {Tuesday, "Tuesday"}, 
    {Wednesday, "Wednesday"}, {Thursday, "Thursday"}, {Friday, "Friday"}};
  auto iterator = day_str_map.find(day_num);
  if (iterator != day_str_map.end())
    return iterator->second;
}

string convert_Time2str(Time time_)
{
  string hour_str, minute_str;
  int hour = get<Hour>(time_), minute = get<Minute>(time_);
  if(hour < 10)
    hour_str = '0' + to_string(hour);
  else
    hour_str = to_string(hour);
  if(minute < 10)
    minute_str = '0' + to_string(minute);
  else
    minute_str = to_string(minute);
  return hour_str + ':' + minute_str;
}

Time make_time(string time_string)
{
  return Time({stoi(time_string.substr(0, 2)), stoi(time_string.substr(3, 2))}); 
}

vector<string> parse_csv_line(const string csv_line)
{
  vector<string> parsed_words;
  string word("");
  for(char character: csv_line)
  {
    if(character == SEPERATE)
    {
      parsed_words.push_back(word);
      word = "";
      continue;
    }
    word = word + character;
  }
  parsed_words.push_back(word);
  return parsed_words;
}

vector<string> get_headings(string csv_headings_line)
{
  return parse_csv_line(csv_headings_line);
}

day make_day(string day_str)
{
  unordered_map<string, day> day_map =
    {{"Saturday", Saturday}, {"Sunday", Sunday}, {"Monday", Monday}, {"Tuesday", Tuesday}, 
    {"Wednesday", Wednesday}, {"Thursday", Thursday}, {"Friday", Friday}};
  auto iterator = day_map.find(day_str);
  if (iterator != day_map.end()) 
    return iterator->second;
}

Movie associate_heading_with_attr(Movie& movie, csv_headings heading, string info)
{
  switch (heading)
  {
    case CinemaName:
      movie.CinemaName = info;
      break;
    case MovieName:
      movie.MovieName = info;
      break;
    case StartingTime:
      movie.StartingTime = make_time(info);
      break;
    case FinishingTime:
      movie.FinishingTime = make_time(info);
      break;
    case Price:
      movie.Price = stoi(info);
      break;
    case Hall:
      movie.Hall = stoi(info);
      break;
    case Day:
      movie.Day = make_day(info);
      break;
  }
  return movie;
}

csv_headings make_heading(string heading_str)
{
  unordered_map<string, csv_headings> csv_headings_map =
    {{"MovieName", MovieName}, {"CinemaName", CinemaName}, {"Day", Day}, {"Hall", Hall}, 
    {"StartingTime", StartingTime}, {"FinishingTime", FinishingTime}, {"Price", Price}};
  auto iterator = csv_headings_map.find(heading_str);
  if (iterator != csv_headings_map.end())
    return iterator->second;
}

Movie make_movie_from_line(vector<string> csv_movie_detailes, const vector<string> headings)
{
  Movie movie;
  for(int index = 0; index < NUM_OF_HEADINGS; index++)
  {
    csv_headings heading = make_heading(headings[index]);
    movie = associate_heading_with_attr(movie, heading, csv_movie_detailes[index]);
  }
  return movie;
}

MovieList parse_csv(string csv_path)
{
  ifstream csv_fileHandle(csv_path, fstream::in);
  string csv_line;
  MovieList movies_list;
  getline(csv_fileHandle, csv_line);
  vector<string> headings = get_headings(csv_line);
  while (getline(csv_fileHandle, csv_line))
  {
    Movie movie = make_movie_from_line(parse_csv_line(csv_line), headings);
    movies_list.push_back(movie);
  }
  return movies_list;
}

tuple<int,string> extract_word(string& line, int current_position)
{
  int index;
  string word;
  for(index = current_position; index < line.size(); ++index)
    if(line[index] != ' ' && line[index] != '\n')
      word = word + line[index];
    else
      break;
  return make_tuple(index + 1, word);
}

vector<string> extract_command_pack(string& line)
{
  tuple<int, string> GET = extract_word(line, 0);
  tuple<int, string> ARG1 = extract_word(line, get<NEXT_WORD_INDEX>(GET));
  int index = get<NEXT_WORD_INDEX>(ARG1);
  string ARG2("");
  if(get<WORD>(ARG1).compare("ALL"))
    while(true)
    {
      ARG2 = ARG2 + line[index];
      index++;
      if(index == line.size())
        break;
    }
  else
    ARG2 = get<WORD>(extract_word(line, get<NEXT_WORD_INDEX>(ARG1)));
  vector<string> command_details {get<WORD>(GET), get<WORD>(ARG1), ARG2};
  return command_details;
}

command_mode authenticate_command(vector<string> potential_command)
{
  if(potential_command[Get].compare("GET"))
    return Invalid;
  if(!potential_command[Arg1].compare("ALL")
      && !potential_command[Arg2].compare("MOVIES"))
    return GetAllMovies;
  if(!potential_command[Arg1].compare("SCHEDULE"))
    return GetSchedule;
  return Invalid;
}

vector<string> get_all_movie_names(const MovieList& movie_list)
{
  vector<string> movie_names;
  for(Movie movie: movie_list)
    movie_names.push_back(movie.MovieName);
  return movie_names;
}

MovieList find_movie_by_name(const MovieList& movies, string movie_name)
{
  MovieList found_movies;
  for(Movie movie: movies)
    if(!movie.MovieName.compare(movie_name))
      found_movies.push_back(movie);
  return found_movies;
}

vector<vector<int>> get_same_Price_indeces(MovieList& movie_list)
{
  vector<vector<int>> same_price_indeces;
  for(int movie_it = 0; movie_it < movie_list.size(); movie_it++)
  {
    Movie movie = movie_list[movie_it];
    if(same_price_indeces.size() == 0 ||
        movie.Price != movie_list[same_price_indeces.back().back()].Price)
      same_price_indeces.push_back(vector<int>({movie_it}));
    else
      same_price_indeces.back().push_back(movie_it);
  }
  return same_price_indeces;
}

vector<vector<int>> get_same_StartTime_indeces(MovieList& movie_list)
{
  vector<vector<int>> same_StartTime_indeces;
  for(int movie_it = 0; movie_it < movie_list.size(); movie_it++)
  {
    Movie movie = movie_list[movie_it];
    if(same_StartTime_indeces.size() == 0 ||
        !times_are_same(movie.StartingTime, movie_list[same_StartTime_indeces.back().back()].StartingTime))
      same_StartTime_indeces.push_back(vector<int>({movie_it}));
    else
      same_StartTime_indeces.back().push_back(movie_it);
  }
  return same_StartTime_indeces;
}

MovieList sort_same_Price_by_CinemaName(MovieList movie_list, vector<vector<int>> same_Price_indeces)
{
  for(vector<int> same_Price_index_pack: same_Price_indeces)
  {
    int iterator = 0;
    MovieList temp_movie_list;

    for(int index_in_pack: same_Price_index_pack)
      temp_movie_list.push_back(movie_list[index_in_pack]);
  
    temp_movie_list = sort_by_CinemaName(temp_movie_list);

    for(int index_in_pack: same_Price_index_pack)
    {
      movie_list[index_in_pack] = temp_movie_list[iterator];
      iterator++;
    }
  }
  return movie_list;
}

MovieList sort_same_StartTimes_by_Price(MovieList movie_list, vector<vector<int>> same_StartTime_indeces)
{
  for(vector<int> same_StartTime_index_pack: same_StartTime_indeces)
  {
    int iterator = 0;
    MovieList temp_movie_list;

    for(int index_in_pack: same_StartTime_index_pack)
      temp_movie_list.push_back(movie_list[index_in_pack]);
    
    temp_movie_list = sort_by_Price(temp_movie_list);
    vector<vector<int>> same_Price_indeces = get_same_Price_indeces(temp_movie_list);
    temp_movie_list = sort_same_Price_by_CinemaName(temp_movie_list, same_Price_indeces);

    for(int index_in_pack: same_StartTime_index_pack)
    {
      movie_list[index_in_pack] = temp_movie_list[iterator];
      iterator++;
    }
  }
  return movie_list;
}

MovieList sort_MovieList(MovieList movie_list)
{
  MovieList sorted;
  movie_list = sort_by_StartTime(movie_list);
  vector<vector<int>> same_StartTime_indeces = get_same_StartTime_indeces(movie_list);
  movie_list = sort_same_StartTimes_by_Price(movie_list, same_StartTime_indeces);
  return movie_list;
}

WeekShowTimes sort_WeekShowTimes(WeekShowTimes showTimes)
{
  for(int day_it = Saturday; day_it < DAYS_IN_WEEK; day_it++)
    showTimes[day_it] = sort_MovieList(showTimes[day_it]);
  return showTimes;
}

MovieList undermine_conflicting_times(MovieList movie_list)
{
  if(movie_list.size() == 0 || movie_list.size() == 1)
    return movie_list;

  MovieList no_conflict_movie_list {movie_list[0]};
  
  for(int current_index = 1; current_index < movie_list.size(); current_index++)
  {
    Movie movie = movie_list[current_index];
    if(first_time_is_earlier(no_conflict_movie_list.back().FinishingTime, movie.StartingTime))
      no_conflict_movie_list.push_back(movie);
  }
  return no_conflict_movie_list;
}

WeekShowTimes make_schedule(WeekShowTimes showTimes)
{
  WeekShowTimes schedule(DAYS_IN_WEEK);
  for(int day_it = Saturday; day_it < DAYS_IN_WEEK; day_it++)
    schedule[day_it] = undermine_conflicting_times(showTimes[day_it]);
  return schedule;
}

WeekShowTimes pack_days(MovieList movie_list)
{
  WeekShowTimes week_show_times(DAYS_IN_WEEK);
  for (Movie movie: movie_list)
    week_show_times[movie.Day].push_back(movie);
  return week_show_times;
}

WeekShowTimes get_schedule(const MovieList& movies, MovieList movie_data)
{
  WeekShowTimes showTimes = sort_WeekShowTimes(pack_days(sort_by_Day(movie_data)));
  WeekShowTimes schedule = make_schedule(showTimes);
  return schedule;
} 

vector<string> filter_duplicates(vector<string> names)
{
  names.erase(unique(names.begin(), names.end()), names.end());
  return names;
}

void stdOut_vector_of_string(vector<string> vec_of_str)
{
  for(string str: vec_of_str)
    cout << str << endl;
}

void RUN_GET_ALL_MOVIES_COMMAND(const MovieList& movies)
{
  MovieList movie_list;
  movie_list = sort_by_MovieName(movies);
  vector<string> movie_names = filter_duplicates(get_all_movie_names(movie_list));
  
  stdOut_vector_of_string(movie_names);
}

Time movie_length(Movie movie)
{
  return make_tuple(get<Hour>(movie.FinishingTime) - get<Hour>(movie.StartingTime),
    get<Minute>(movie.FinishingTime) - get<Minute>(movie.StartingTime));
}

int half_hours_in(Time length)
{
  return get<Hour>(length) * 2 + (get<Minute>(length) == 30);
}

string put_key_times()
{
  string line("          ");
  Time key_time = make_time("08:00");
  for(int index = 0; index < 8; index++)
  {
    line = line + convert_Time2str(key_time) + "               ";
    key_time = make_tuple(get<Hour>(key_time) + 2, get<Minute>(key_time));
  }
  line = line + "00:00";
  return line;
}

int find_start_pillar(Movie movie)
{
  Time start = movie.StartingTime;
  return half_hours_in(make_tuple(get<Hour>(start) - 8, get<Minute>(start))) * 5 + 10;
}

int find_end_pillar(int start_pillar, int space)
{
  return start_pillar + space + 1;
}

string make_line()
{
  string line("");
  for(int index = 0; index < 175; index++)
    line = line + ' ';
  return line;
}

void put_pillars_and_CinemaName(Movie movie, ConsoleMap& console_map, int row, int spaces)
{
  int iterator = 0;
  int start_pillar = find_start_pillar(movie);
  int end_pillar = find_end_pillar(start_pillar, spaces);
  console_map[row][start_pillar] = '|';
  for(int index = start_pillar + 1; index < end_pillar; index++)
    if(index < start_pillar + movie.CinemaName.size() + 1)
    {
      console_map[row][index] = movie.CinemaName[iterator];
      iterator++;
    }
    else
      console_map[row][index] = '*';
  console_map[row][end_pillar] = '|';
}

void put_day_name(ConsoleMap& console_map, int day_it, int row)
{
  string day_name = convert_day2str(static_cast<day>(day_it));
  int iterator = 0;
  for(int index = 0; index < day_name.size(); index++)
  {
    console_map[row][index] = day_name[iterator];
    iterator++;
  }
}

void remove_asterisks(ConsoleMap& console_map)
{
  for(int row_index = 0; row_index < 16; row_index++)
    for(int col_index = 0; col_index < 175; col_index++)
      if(console_map[row_index][col_index] == '*')
        console_map[row_index][col_index] = ' ';
}

void put_roof_ceiling_and_junction(ConsoleMap& console_map, int row)
{
  for(int col_index = 0; col_index < 175; col_index++)
  {
    if((col_index > 9 && isalpha(console_map[row][col_index])) ||
      (console_map[row][col_index] == '*' && console_map[row][col_index] != '|'))
    {
      if(console_map[row - 1][col_index] != '+') console_map[row - 1][col_index] = '-';
      if(console_map[row + 1][col_index] != '+') console_map[row + 1][col_index] = '-';
    }
    if(console_map[row][col_index] == '|')
    {
      console_map[row - 1][col_index] = '+';
      console_map[row + 1][col_index] = '+';
    }
  }
  remove_asterisks(console_map);
}

ConsoleMap make_console_map(WeekShowTimes schedule, Time Movie_length)
{
  int row, spaces_between_pillars = half_hours_in(Movie_length) * 5 - 1;
  ConsoleMap console_map(16,make_line());
  console_map[0] = put_key_times();
  for(int day_it = Saturday; day_it < DAYS_IN_WEEK; day_it++)
  {
    row = 2 * (day_it + 1);
    put_day_name(console_map, day_it, row);
    for(Movie movie: schedule[day_it])
      put_pillars_and_CinemaName(movie, console_map, row, spaces_between_pillars);
    put_roof_ceiling_and_junction(console_map, row);
  }
  return console_map;
}

string put_time_boxes()
{
  stringstream line;
  Time key_time = make_time("08:00");
  for(int index = 0; index < 8; index++)
  {
    line << "<div class=\"time-box\" style=\"left: " << to_string((2 * index + 1) * 100)
      << "px;\"><p>" << convert_Time2str(key_time) << "</p></div>" << endl;
    key_time = make_tuple(get<Hour>(key_time) + 2, get<Minute>(key_time));
  }
  line << "<div class=\"time-box\" style=\"left: 1700px;\"><p>00:00</p></div>" << endl;
  return line.str();
}

string put_vertical_lines()
{
  stringstream line;
  for(int index = 0; index < 17; index++)
    line << "<div class=\"vertical-line\" style=\"left: " << to_string((index + 1) * 100)
      << "px;\"></div>" << endl;
  return line.str();
}

string put_day_boxes()
{
  stringstream line;
  for(int index = 0; index < 7; index++)
    line << "<div class=\"day-box\" style=\"top: " << to_string(index * 50 + 60)
      << "px;\">" << convert_day2str(static_cast<day>(index)) << "</div>" << endl;
  return line.str();
}

string put_pre_stuff()
{
  stringstream line;
  line << "<html lang=\"en\">" << endl << "<head>" << endl << "<title></title>" << endl
  << "<link rel=\"stylesheet\" href=\"./style.css\" />" << endl << "</head>" << endl
  << "<body>" << endl;
  return line.str();
}

void write_to_html_file(string content, string file_name)
{
  ofstream output_fileHandle(file_name, ios::out);
  output_fileHandle << content << endl;
  output_fileHandle.close();
}

string put_width(Movie movie)
{
  return to_string(50 * half_hours_in(movie_length(movie)));
}

string put_left(Movie movie)
{
  return to_string(100 + 50 * half_hours_in(make_tuple(
    get<Hour>(movie.StartingTime) - 8, get<Minute>(movie.StartingTime))));
}

string put_top(Movie movie)
{
  return to_string(60 + static_cast<int>(movie.Day) * 50);
}

string put_movie_schedule(WeekShowTimes schedule)
{
  stringstream line;
  for(int day_it = 0; day_it < DAYS_IN_WEEK; day_it++)
  {
    for(Movie movie: schedule[day_it])
    {
      line << "<div class=\"record-box\" style=\"width: "
        << put_width(movie) << "px; left: " << put_left(movie) << "px; top:"
        << put_top(movie) << "px; \">" << movie.CinemaName << "</div>" << endl;
    }
  }
  return line.str();
}

void make_html_file(WeekShowTimes schedule, string html_fileName)
{
  stringstream html_contents;
  html_contents << put_pre_stuff() << put_time_boxes() << endl
    << put_vertical_lines() << endl << put_day_boxes() << endl
    << put_movie_schedule(schedule) << endl << "</body>" << endl
    << "</html>" << endl;
  write_to_html_file(html_contents.str(), html_fileName);
}

void RUN_GET_SCHEDULE_COMMAND(MovieList& movies, string movie_name)
{
  MovieList movie_data = find_movie_by_name(movies, movie_name);
  if(movie_data.size() == 0)
    return;
  
  WeekShowTimes schedule = get_schedule(movies, movie_data);

  ConsoleMap console_map = make_console_map(schedule, movie_length(movie_data[0]));
  stdOut_vector_of_string(console_map);

  string html_fileName = movie_name + ".html";
  make_html_file(schedule, html_fileName);
}

void command_handler(MovieList& movie_list, command_mode& command, string movie_name)               
{ 
  switch(command)
  {
    case Invalid:
      return;
    case GetAllMovies:
      RUN_GET_ALL_MOVIES_COMMAND(movie_list);
      break;
    case GetSchedule:
      RUN_GET_SCHEDULE_COMMAND(movie_list, movie_name);
      break;
  }
  command = Invalid;
}

void get_and_redirect_command(MovieList& movie_list)
{
  string line;
  while(getline(cin, line))
  {
    vector<string> potential_command = extract_command_pack(line);
    command_mode command = authenticate_command(potential_command);
    command_handler(movie_list, command, potential_command[2]);
  }
}

int main(int argc, char* argv[])
{
  MovieList movie_list = parse_csv(argv[CSV_path]);
  get_and_redirect_command(movie_list);
  return 0;
}