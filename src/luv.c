#include "luv.h"
#include "uv.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "luv_fs.h"
#include "luv_handle.h"
#include "luv_tcp.h"

////////////////////////////////////////////////////////////////////////////////
//                              ref structs                                   //
////////////////////////////////////////////////////////////////////////////////


typedef struct {
  lua_State* L;
  int r;
  uv_write_t write_req;
  uv_buf_t refbuf;
} luv_write_ref_t;

typedef struct {
  lua_State* L;
  int r;
  uv_shutdown_t shutdown_req;
} luv_shutdown_ref_t;



////////////////////////////////////////////////////////////////////////////////
//                             utility functions                              //
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//                        event callback dispatchers                          //
////////////////////////////////////////////////////////////////////////////////

void luv_on_connection(uv_stream_t* handle, int status) {
  // load the lua state and the userdata
  luv_ref_t* ref = handle->data;
  lua_State *L = ref->L;
  int before = lua_gettop(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, ref->r);

  lua_pushinteger(L, status);
  luv_emit_event(L, "connection", 1);
  lua_pop(L, 1); // remove the userdata
  assert(lua_gettop(L) == before);
}

void luv_on_read(uv_stream_t* handle, ssize_t nread, uv_buf_t buf) {

  // load the lua state and the userdata
  luv_ref_t* ref = handle->data;
  lua_State *L = ref->L;
  int before = lua_gettop(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, ref->r);

  if (nread >= 0) {

    lua_pushlstring (L, buf.base, nread);
    lua_pushinteger (L, nread);
    luv_emit_event(L, "read", 2);

  } else {
    uv_err_t err = uv_last_error(uv_default_loop());
    if (err.code == UV_EOF) {
      luv_emit_event(L, "end", 0);
    } else {
      error(L, "read: %s", uv_strerror(err));
    }
  }
  lua_pop(L, 1); // Remove the userdata

  free(buf.base);
  assert(lua_gettop(L) == before);
}



void luv_after_shutdown(uv_shutdown_t* req, int status) {

  // load the lua state and the callback
  luv_shutdown_ref_t* ref = req->data;
  lua_State *L = ref->L;
  int before = lua_gettop(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, ref->r);
  luaL_unref(L, LUA_REGISTRYINDEX, ref->r);

  lua_pushnumber(L, status);
  if (lua_pcall(L, 1, 0, 0) != 0) {
    error(L, "error running function 'on_shutdown': %s", lua_tostring(L, -1));
  }
  free(ref);// We're done with the ref object, free it
  assert(lua_gettop(L) == before);
}

void luv_after_write(uv_write_t* req, int status) {

  // load the lua state and the callback
  luv_write_ref_t* ref = req->data;
  lua_State *L = ref->L;
  int before = lua_gettop(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, ref->r);
  luaL_unref(L, LUA_REGISTRYINDEX, ref->r);
  lua_pushnumber(L, status);
  if (lua_pcall(L, 1, 0, 0) != 0) {
    error(L, "error running function 'on_write': %s", lua_tostring(L, -1));
  }
  free(ref);// We're done with the ref object, free it
  assert(lua_gettop(L) == before);

}



////////////////////////////////////////////////////////////////////////////////
//                               Constructors                                 //
////////////////////////////////////////////////////////////////////////////////

static int luv_new_udp (lua_State* L) {
  int before = lua_gettop(L);

  //uv_udp_t* handle = (uv_udp_t*)
  lua_newuserdata(L, sizeof(uv_udp_t));

  // Set metatable for type
  luaL_getmetatable(L, "luv_udp");
  lua_setmetatable(L, -2);

  // Create a local environment for storing stuff
  lua_newtable(L);
  lua_setfenv (L, -2);

  assert(lua_gettop(L) == before + 1);

  return 1;
}

static int luv_new_pipe (lua_State* L) {
  int before = lua_gettop(L);

  //uv_pipe_t* handle = (uv_pipe_t*)
  lua_newuserdata(L, sizeof(uv_pipe_t));

  // Set metatable for type
  luaL_getmetatable(L, "luv_pipe");
  lua_setmetatable(L, -2);

  // Create a local environment for storing stuff
  lua_newtable(L);
  lua_setfenv (L, -2);

  assert(lua_gettop(L) == before + 1);

  return 1;
}

static int luv_new_tty (lua_State* L) {
  int before = lua_gettop(L);

  //uv_tty_t* handle = (uv_tty_t*)
  lua_newuserdata(L, sizeof(uv_tty_t));

  // Set metatable for type
  luaL_getmetatable(L, "luv_pipe");
  lua_setmetatable(L, -2);

  // Create a local environment for storing stuff
  lua_newtable(L);
  lua_setfenv (L, -2);

  assert(lua_gettop(L) == before + 1);

  return 1;
}

////////////////////////////////////////////////////////////////////////////////
//                              UDP Functions                                 //
////////////////////////////////////////////////////////////////////////////////

static int luv_udp_init(lua_State* L) {
  error(L, "TODO: Implement luv_udp_init");
  return 0;
}

static int luv_udp_bind(lua_State* L) {
  error(L, "TODO: Implement luv_udp_bind");
  return 0;
}

static int luv_udp_bind6(lua_State* L) {
  error(L, "TODO: Implement luv_udp_bind6");
  return 0;
}

static int luv_udp_getsockname(lua_State* L) {
  error(L, "TODO: Implement luv_udp_getsockname");
  return 0;
}

static int luv_udp_send(lua_State* L) {
  error(L, "TODO: Implement luv_udp_send");
  return 0;
}

static int luv_udp_send6(lua_State* L) {
  error(L, "TODO: Implement luv_udp_send6");
  return 0;
}

static int luv_udp_recv_start(lua_State* L) {
  error(L, "TODO: Implement luv_udp_recv_start");
  return 0;
}

static int luv_udp_recv_stop(lua_State* L) {
  error(L, "TODO: Implement luv_udp_recv_stop");
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//                             Stream Functions                               //
////////////////////////////////////////////////////////////////////////////////

static int luv_shutdown(lua_State* L) {
  int before = lua_gettop(L);
  uv_stream_t* handle = (uv_stream_t*)luv_checkudata(L, 1, "stream");
  luaL_checktype(L, 2, LUA_TFUNCTION);
  luv_shutdown_ref_t* ref = (luv_shutdown_ref_t*)malloc(sizeof(luv_shutdown_ref_t));

  // Store a reference to the callback
  ref->L = L;
  lua_pushvalue(L, 2);
  ref->r = luaL_ref(L, LUA_REGISTRYINDEX);

  // Give the shutdown_req access to this
  ref->shutdown_req.data = ref;

  uv_shutdown(&ref->shutdown_req, handle, luv_after_shutdown);

  assert(lua_gettop(L) == before);
  return 0;
}

static int luv_listen (lua_State* L) {
  int before = lua_gettop(L);
  uv_stream_t* handle = (uv_stream_t*)luv_checkudata(L, 1, "stream");
  luaL_checktype(L, 2, LUA_TFUNCTION);

  luv_register_event(L, 1, "connection", 2);

  if (uv_listen(handle, 128, luv_on_connection)) {
    uv_err_t err = uv_last_error(uv_default_loop());
    error(L, "listen: %s", uv_strerror(err));
  }

  assert(lua_gettop(L) == before);
  return 0;
}

static int luv_accept (lua_State* L) {
  int before = lua_gettop(L);
  uv_stream_t* server = (uv_stream_t*)luv_checkudata(L, 1, "stream");
  uv_stream_t* client = (uv_stream_t*)luv_checkudata(L, 2, "stream");
  if (uv_accept(server, client)) {
    uv_err_t err = uv_last_error(uv_default_loop());
    error(L, "accept: %s", uv_strerror(err));
  }

  assert(lua_gettop(L) == before);
  return 0;
}

static int luv_read_start (lua_State* L) {
  int before = lua_gettop(L);
  uv_stream_t* handle = (uv_stream_t*)luv_checkudata(L, 1, "stream");
  uv_read_start(handle, luv_on_alloc, luv_on_read);
  assert(lua_gettop(L) == before);
  return 0;
}

static int luv_read_stop(lua_State* L) {
  int before = lua_gettop(L);
  uv_stream_t* handle = (uv_stream_t*)luv_checkudata(L, 1, "stream");
  uv_read_stop(handle);
  assert(lua_gettop(L) == before);
  return 0;
}

static int luv_write (lua_State* L) {
  int before = lua_gettop(L);
  uv_stream_t* handle = (uv_stream_t*)luv_checkudata(L, 1, "stream");
  size_t len;
  const char* chunk = luaL_checklstring(L, 2, &len);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  luv_write_ref_t* ref = (luv_write_ref_t*)malloc(sizeof(luv_write_ref_t));

  // Store a reference to the callback
  ref->L = L;
  lua_pushvalue(L, 3);
  ref->r = luaL_ref(L, LUA_REGISTRYINDEX);

  // Give the write_req access to this
  ref->write_req.data = ref;

  // Store the chunk
  // TODO: this is probably unsafe, should investigate
  ref->refbuf.base = (char*)chunk;
  ref->refbuf.len = len;

  uv_write(&ref->write_req, handle, &ref->refbuf, 1, luv_after_write);
  assert(lua_gettop(L) == before);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//                              Pipe Functions                                //
////////////////////////////////////////////////////////////////////////////////

static int luv_pipe_init(lua_State* L) {
  error(L, "TODO: Implement luv_pipe_init");
  return 0;
}

static int luv_pipe_open(lua_State* L) {
  error(L, "TODO: Implement luv_pipe_open");
  return 0;
}

static int luv_pipe_bind(lua_State* L) {
  error(L, "TODO: Implement luv_pipe_bind");
  return 0;
}

static int luv_pipe_connect(lua_State* L) {
  error(L, "TODO: Implement luv_pipe_connect");
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//                              TTY Functions                                 //
////////////////////////////////////////////////////////////////////////////////

static int luv_tty_init(lua_State* L) {
  error(L, "TODO: Implement luv_tty_init");
  return 0;
}

static int luv_tty_set_mode(lua_State* L) {
  error(L, "TODO: Implement luv_tty_set_mode");
  return 0;
}

static int luv_tty_get_winsize(lua_State* L) {
  error(L, "TODO: Implement luv_tty_get_winsize");
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//                              Loop Functions                                //
////////////////////////////////////////////////////////////////////////////////

static int luv_run (lua_State* L) {
  uv_run(uv_default_loop());
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

static const luaL_reg luv_f[] = {
  // Constructors
  {"new_udp", luv_new_udp},
  {"new_tcp", luv_new_tcp},
  {"new_pipe", luv_new_pipe},
  {"new_tty", luv_new_tty},

  // Handle functions
  {"close", luv_close},
  {"set_handler", luv_set_handler},

  // UDP functions
  {"udp_init", luv_udp_init},
  {"udp_bind", luv_udp_bind},
  {"udp_bind6", luv_udp_bind6},
  {"udp_getsockname", luv_udp_getsockname},
  {"udp_send", luv_udp_send},
  {"udp_send6", luv_udp_send6},
  {"udp_recv_start", luv_udp_recv_start},
  {"udp_recv_stop", luv_udp_recv_stop},

  // Stream functions
  {"shutdown", luv_shutdown},
  {"listen", luv_listen},
  {"accept", luv_accept},
  {"read_start", luv_read_start},
  {"read_stop", luv_read_stop},
  {"write", luv_write},

  // TCP functions
  {"tcp_init", luv_tcp_init},
  {"tcp_bind", luv_tcp_bind},
  {"tcp_bind6", luv_tcp_bind6},
  {"tcp_getsockname", luv_tcp_getsockname},
  {"tcp_getpeername", luv_tcp_getpeername},
  {"tcp_connect", luv_tcp_connect},
  {"tcp_connect6", luv_tcp_connect6},

  // Pipe functions
  {"pipe_init", luv_pipe_init},
  {"pipe_open", luv_pipe_open},
  {"pipe_bind", luv_pipe_bind},
  {"pipe_connect", luv_pipe_connect},

  // TTY functions
  {"tty_init", luv_tty_init},
  {"tty_set_mode", luv_tty_set_mode},
  {"tty_get_winsize", luv_tty_get_winsize},

  // FS functions
  {"fs_open", luv_fs_open},
  {"fs_close", luv_fs_close},
  {"fs_read", luv_fs_read},
  {"fs_write", luv_fs_write},
  {"fs_unlink", luv_fs_unlink},
  {"fs_mkdir", luv_fs_mkdir},
  {"fs_rmdir", luv_fs_rmdir},

  // Loop functions
  {"run", luv_run},
  {NULL, NULL}
};

static const luaL_reg luv_handle_m[] = {
  {"close", luv_close},
  {"set_handler", luv_set_handler},
  {NULL, NULL}
};

static const luaL_reg luv_udp_m[] = {
  {"bind", luv_udp_bind},
  {"bind6", luv_udp_bind6},
  {"getsockname", luv_udp_getsockname},
  {"send", luv_udp_send},
  {"send6", luv_udp_send6},
  {"recv_start", luv_udp_recv_start},
  {"recv_stop", luv_udp_recv_stop},
  {NULL, NULL}
};

static const luaL_reg luv_stream_m[] = {
  {"shutdown", luv_shutdown},
  {"listen", luv_listen},
  {"accept", luv_accept},
  {"read_start", luv_read_start},
  {"read_stop", luv_read_stop},
  {"write", luv_write},
  {NULL, NULL}
};

static const luaL_reg luv_tcp_m[] = {
  {"init", luv_tcp_init},
  {"bind", luv_tcp_bind},
  {"bind6", luv_tcp_bind6},
  {"getsockname", luv_tcp_getsockname},
  {"getpeername", luv_tcp_getpeername},
  {"connect", luv_tcp_connect},
  {"connect6", luv_tcp_connect6},
  {NULL, NULL}
};

static const luaL_reg luv_pipe_m[] = {
  {"open", luv_pipe_open},
  {"bind", luv_pipe_bind},
  {"connect", luv_pipe_connect},
  {NULL, NULL}
};

static const luaL_reg luv_tty_m[] = {
  {"tty_set_mode", luv_tty_set_mode},
  {"tty_get_winsize", luv_tty_get_winsize},
  {NULL, NULL}
};

LUALIB_API int luaopen_uv (lua_State* L) {
  int before = lua_gettop(L);

  // metatable for handle userdata types
  // It is it's own __index table to save space
  luaL_newmetatable(L, "luv_handle");
  luaL_register(L, NULL, luv_handle_m);
  lua_pushboolean(L, TRUE);
  lua_setfield(L, -2, "is_handle"); // Tag for polymorphic type checking
  lua_pushvalue(L, -1); // copy the metatable/table so it's still on the stack
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  // Metatable for udp
  luaL_newmetatable(L, "luv_udp");
  // Create table of udp methods
  lua_newtable(L); // udp_m
  luaL_register(L, NULL, luv_udp_m);
  lua_pushboolean(L, TRUE);
  lua_setfield(L, -2, "is_udp"); // Tag for polymorphic type checking
  // Load the parent metatable so we can inherit it's methods
  luaL_newmetatable(L, "luv_handle");
  lua_setmetatable(L, -2);
  // use method table in metatable's __index
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1); // we're done with luv_udp

  // Metatable for streams
  luaL_newmetatable(L, "luv_stream");
  // Create table of stream methods
  lua_newtable(L); // stream_m
  luaL_register(L, NULL, luv_stream_m);
  lua_pushboolean(L, TRUE);
  lua_setfield(L, -2, "is_stream"); // Tag for polymorphic type checking
  // Load the parent metatable so we can inherit it's methods
  luaL_newmetatable(L, "luv_handle");
  lua_setmetatable(L, -2);
  // use method table in metatable's __index
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1); // we're done with luv_stream

  // metatable for tcp userdata
  luaL_newmetatable(L, "luv_tcp");
  // table for methods
  lua_newtable(L); // tcp_m
  luaL_register(L, NULL, luv_tcp_m);
  lua_pushboolean(L, TRUE);
  lua_setfield(L, -2, "is_tcp"); // Tag for polymorphic type checking
  // Inherit from streams
  luaL_newmetatable(L, "luv_stream");
  lua_setmetatable(L, -2);
  // Use as __index and pop metatable
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1); // we're done with luv_tcp

  // metatable for pipe userdata
  luaL_newmetatable(L, "luv_pipe");
  // table for methods
  lua_newtable(L); // pipe_m
  luaL_register(L, NULL, luv_pipe_m);
  lua_pushboolean(L, TRUE);
  lua_setfield(L, -2, "is_pipe"); // Tag for polymorphic type checking
  // Inherit from streams
  luaL_newmetatable(L, "luv_stream");
  lua_setmetatable(L, -2);
  // Use as __index and pop metatable
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1); // we're done with luv_pipe

  // metatable for tty userdata
  luaL_newmetatable(L, "luv_tty");
  // table for methods
  lua_newtable(L); // tty_m
  luaL_register(L, NULL, luv_tty_m);
  lua_pushboolean(L, TRUE);
  lua_setfield(L, -2, "is_tty"); // Tag for polymorphic type checking
  // Inherit from streams
  luaL_newmetatable(L, "luv_stream");
  lua_setmetatable(L, -2);
  // Use as __index and pop metatable
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1); // we're done with luv_tty


  // Create a new exports table with functions and constants
  lua_newtable (L);
  luaL_register(L, NULL, luv_f);
  lua_pushnumber(L, UV_VERSION_MAJOR);
  lua_setfield(L, -2, "VERSION_MAJOR");
  lua_pushnumber(L, UV_VERSION_MINOR);
  lua_setfield(L, -2, "VERSION_MINOR");

  assert(lua_gettop(L) == before + 1);
  return 1;
}

