#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include "gtp.h"

#define FOREACH BOOST_FOREACH

namespace Gtp {

Io::Io (istringstream& arg_line) : in (arg_line) {
}

void Io::CheckEmpty() {
  string s;
  in >> s;
  if (in) throw syntax_error;
  in.clear();
}

bool Io::IsEmpty() {
  string s;
  std::streampos pos = in.tellg();
  in >> s;
  bool ok = in;
  in.seekg(pos);
  in.clear();
  return !ok;
}

istream& Io::In () {
  return in;
}

ostream& Io::Out () {
  return out;
}

const Error syntax_error("syntax error");

// -----------------------------------------------------------------------------

// IMPROVE: can't Static Aux be anonymous function

void StaticAux(const string& ret, Io& io) {
  io.CheckEmpty ();
  io.Out() << ret;
}

Callback StaticCommand (const string& ret) {
  return boost::bind(StaticAux, ret, _1);
}

// -----------------------------------------------------------------------------

Repl::Repl () {
  RegisterCommand ("list_commands",
                  OfMethod(this, &Repl::CListCommands));
  RegisterCommand ("help",
                  OfMethod(this, &Repl::CListCommands));
  RegisterCommand ("known_command",
                  OfMethod(this, &Repl::CKnownCommand));
  RegisterCommand ("quit",
                  OfMethod(this, &Repl::CQuit));
}

void Repl::RegisterCommand (const string& name, Callback callback) {
  assert(!IsCommand(name));
  callbacks[name] = callback;
}

void Preprocess (string* line) {
  ostringstream ret;
  FOREACH (char c, *line) {
    if (c == 9) ret << '\32';
    else if (c > 0   && c <= 9)  continue;
    else if (c >= 11 && c <= 31) continue;
    else if (c == 127) continue;
    else if (c == '#') break;  // remove comments
    else ret << c;
  }
  *line = ret.str ();
}

void Repl::Run (istream& in, ostream& out) {
  in.clear();
  while (true) {
    string line;
    if (!getline (in, line)) break;
    Preprocess(&line);
    istringstream line_stream (line);

    string cmd_name;
    if (!(line_stream >> cmd_name)) continue; // empty line

    if (!IsCommand (cmd_name)) {
      Report (out, false, "unknown command: \"" + cmd_name + "\"");
      continue;
    }

    try {
      Io io(line_stream);
      callbacks [cmd_name] (io); // callback call
      Report (out, true, io.out.str());
    }
    catch (Error e) { Report (out, false, e.msg); }
    catch (Quit)    { Report (out, true, "bye"); break; }
  }
}

void Repl::Report (ostream& out, bool success, const string& msg) {
  out << (success ? "=" : "?") << " "
      << boost::trim_right_copy(msg) // remove bad endl in msg
      << endl << endl;
}

void Repl::CListCommands (Io& io) {
  io.CheckEmpty();
  pair<string, Callback> p;
  FOREACH(p, callbacks) {
    io.Out() << p.first << endl;
  }
}

void Repl::CKnownCommand (Io& io) {
  io.Out() << boolalpha << IsCommand(io.Read<string> ());
  io.CheckEmpty();
}

void Repl::CQuit (Io& io) {
  io.CheckEmpty();
  throw Quit();
}

bool Repl::IsCommand (const string& name) {
  return callbacks.find(name) != callbacks.end();
}

} // end of namespace Gtp