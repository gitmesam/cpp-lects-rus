#ifndef SDLUTIL_GUARD_
#define SDLUTIL_GUARD_

#include <cstdlib>
#include <cmath>
#include <SDL/SDL.h>

#include <functional>

using namespace std;

enum class pollres 
{
  PROCEED,
  STOP
};

struct ISurface
{
  virtual void putpixel(int x, int y, Uint32 color) = 0;
  virtual void putlogpixel (double x, double y, Uint32 color) = 0;
  virtual void fillwith (Uint32 color) = 0;
  virtual unsigned w () const = 0;
  virtual unsigned h () const = 0;
};

class SDLSurface : public ISurface
{
  SDL_Surface *s;
public:
  SDLSurface (SDL_Surface *surface) : s (surface) {}
  unsigned w() const override { return s->w; }
  unsigned h() const override { return s->h; }
  void putpixel (int x, int y, Uint32 color) override;
  void putlogpixel (double x, double y, Uint32 color) override;
  void fillwith (Uint32 color) override;
};

class ViewPort
{
  int width, height;
  SDL_Surface *screen;
  std::function<void(ISurface*)> callback;
  static ViewPort *v;

  ViewPort (int w, int h, std::function<void(ISurface*)> c) 
    : width (w), height (h), callback(c)
  {   
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
      throw runtime_error (SDL_GetError());

    atexit(SDL_Quit);    
    screen = SDL_SetVideoMode(width + 1, height + 1, 0, SDL_DOUBLEBUF);
    if (screen == NULL)
      throw runtime_error (SDL_GetError());
  }

public:
  pollres poll ();
  void dump (const char *name);

  static ViewPort *
  QueryViewPort (int w, int h, std::function<void(ISurface*)> c)
  {
    if (!v) {
      v = new ViewPort(w, h, c);
    }
    else {
      SDL_FreeSurface(v->screen);    
      v->width = w;
      v->height = h;
      v->callback = c;
      v->screen = SDL_SetVideoMode(w + 1, h + 1, 0, SDL_DOUBLEBUF);
      if (v->screen == NULL)
        throw runtime_error (SDL_GetError());
    }
    return v;
  }

  ~ViewPort()
  {
    SDL_FreeSurface(screen);    
  }
};

#endif
