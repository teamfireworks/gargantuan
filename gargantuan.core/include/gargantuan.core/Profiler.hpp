#pragma once

#if defined(TRACY_ENABLE)
	#include <tracy/Tracy.hpp>
#endif

namespace gargantuan {
	// The engine's zones go to Tracy. Nothing is collected until its profiler
	// connects, so a build with this on and nobody watching costs an atomic
	// read per zone.
	//
	// Run the engine, then `tracy-profiler` and hit Connect. The tools are
	// built by configuring with -DGARGANTUAN_TRACY_TOOLS=ON.

#if defined(TRACY_ENABLE)
	// A scope's worth of time, under whatever zone is already open on this
	// thread. The name must be a literal: Tracy registers the source location
	// once and sends a reference to it, not the string.
	#define G_PROFILE(name) ZoneScopedN(name)

	// For a zone whose name is only known once it runs -- a Luau chunk being
	// the one that is. Costs a copy of the text every call, which is why it is
	// not what G_PROFILE does.
	//
	// The length is checked because the caller only looks a name up when
	// something is listening, and whether that is true is decided one frame and
	// acted on the next. Handing Tracy a zero-length name on the frame the
	// profiler attaches loses the zone and everything nested inside it.
	#define G_PROFILE_NAMED(fallback, text, length) \
		ZoneScopedN(fallback);                      \
		do {                                        \
			if ((length) > 0) {                     \
				ZoneName(text, length);             \
			}                                       \
		} while (0)

	// Ends the frame. Everything between two of these is one row on the frame
	// bar, which is what makes a single bad frame findable.
	#define G_PROFILE_FRAME() FrameMark

	// Whether anything is listening, for work only worth doing when it is --
	// looking up the name above being the case here.
	#define G_PROFILE_ACTIVE() TracyIsConnected
#else
	#define G_PROFILE(name) ((void)0)
	#define G_PROFILE_NAMED(fallback, text, length) ((void)0)
	#define G_PROFILE_FRAME() ((void)0)
	#define G_PROFILE_ACTIVE() false
#endif
} // namespace gargantuan
