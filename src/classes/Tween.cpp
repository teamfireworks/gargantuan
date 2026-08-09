// #include "gargantuan/classes/Tween.hpp"
// #include "gargantuan/classes/Instance.hpp"
// #include "gargantuan/reflection/InstanceClassRegistry.hpp"

// #include <lua.h>

// namespace gargantuan {
// 	G_INSTANCE_IMPL(
// 		Tween,
// 		.Properties =
// 			{
// 				{"Instance", Property::fromMember<&Tween::Instance>(true, false)},
// 				{"TweenInfo", Property::fromMember<&Tween::TweenInfo>(true, false)},
// 				{"PlaybackState", Property::fromMember<&Tween::PlaybackState>(true, false)},
// 			},
// 		.Methods = {
// 			// G_UD_METHOD(Tween, Pause),
// 			// G_UD_METHOD(Tween, Cancel),
// 			// {"Play", Method{&Tween::LPlay}},
// 		},
// 	);

// 	int Tween::LPlay(lua_State *L, gargantuan::Instance *self) {
// 		// Tween *tween = self->Cast<Tween>();

// 		// if (tween->IsPlaying) return 0;

// 		// tween->IsPlaying = true;
// 		// tween->StartTime = lua_clock();
// 		// tween->LerpStartTime = tween->StartTime + tween->TweenInfo.DelayTime;
// 		// tween->EndTime = tween->LerpStartTime + tween->TweenInfo.Time * (tween->TweenInfo.RepeatCount + 1);

// 		// tween->PlaybackState =
// 		// 	tween->TweenInfo.DelayTime > 0 ? Enums::PlaybackState::Delayed : Enums::PlaybackState::Playing;

// 		// if (tween->InitialProperties.empty()) {
// 		// 	auto instance = tween->Instance.get();
// 		// 	for (auto &[name, _] : tween->goalProperties) {
// 		// 		auto property = tween->Instance->FindProperty(name);
// 		// 		if (!property) continue;
// 		// 		if (!property->Read || !property->Write) continue;
// 		// 		// This will so not work
// 		// 		tween->InitialProperties[name] = property->Read(L, instance);
// 		// 	}
// 		// }

// 		return 0;
// 	}

// 	// void Tween::Pause() {
// 	// 	if (PlaybackState != Enums::PlaybackState::Playing) {
// 	// 		return;
// 	// 	}

// 	// 	Paused = true;
// 	// 	PlaybackState = Enums::PlaybackState::Paused;
// 	// }

// 	// void Tween::Cancel() {
// 	// 	Cancelled = true;
// 	// 	Paused = false;
// 	// 	Elapsed = 0.0f;
// 	// 	PlaybackState = Enums::PlaybackState::Cancelled;

// 	// 	Completed->Fire(PlaybackState);
// 	// }

// 	// int Tween::LStep(lua_State *L, gargantuan::Instance *self) {
// 	// 	if (PlaybackState == Enums::PlaybackState::Delayed) {
// 	// 		DelayElapsed += deltaTime;
// 	// 		return;
// 	// 	} else if (PlaybackState != Enums::PlaybackState::Playing) {
// 	// 		return;
// 	// 	}

// 	// 	Elapsed += deltaTime;

// 	// 	auto endTime = Elapsed + TweenInfo.Time;
// 	// 	auto progress = Elapsed / endTime;
// 	// 	auto alpha = EasingCurves::CalculateAlpha(progress);

// 	// 	for (auto &[name, goalValue] : goalProperties) {
// 	// 		InitialProperties[name];
// 	// 	}
// 	// }
// }
