#ifndef _ECHOAUDIOPLAYER_H_
#define _ECHOAUDIOPLAYER_H_
#include <echo/Types.h>
#include <echo/Kernel/Task.h>
#include <echo/FileSystem/File.h>
#include <echo/cpp/functional>
#include <string>
#include <map>

namespace Echo
{
	class AudioBuffer;
	class AudioSource;
	class AudioStream;
	class Audio;
	class FileSystem;

	/**
	 * An AudioPlayer is a higher level way to use audio.
	 * Use an AudioPlayer if you want to play music and sounds but not worry about
	 * any of the details of dealing with opening files, creating AudioSources and
	 * managing audio instances.
	 * 
	 * An AudioPlayer also provides basic fade in and fade out features for music.
	 * 
	 * By default the AudioPlayer supports the following Audio formats:
	 *	- ogg
	 *	- wav
	 * You can add additional file format support by adding additional
	 * AudioSourceLoader functions to the player.
	 */
	class AudioPlayer : public Task
	{
	public:
		///AudioSourceLoaders create a specialisation of AudioSrouce.
		typedef function< shared_ptr<AudioSource>(File) > AudioSourceLoader;
		
		AudioPlayer(shared_ptr<Audio> audio, shared_ptr<FileSystem> fileSystem);
		virtual ~AudioPlayer();

		void OnResume(bool) override;
		void OnPause(bool) override;

		struct CurrentTrackOptions
		{
			enum _
			{
				FADE_LAST,
				FADE_ALL,
				STOP_LAST,
				STOP_ALL,
				DO_NOTHING
			};
		};
		typedef CurrentTrackOptions::_ CurrentTrackOption;
		struct PlayOptions
		{
			enum _
			{
				NOTHING,
				FADE_IN
			};
		};
		typedef PlayOptions::_ PlayOption;
		
		/**
		 * Load a list of available audio to play.
		 * The file can list sounds and tracks. For example:
		 *  sound=path/to/soundfile.wav,Name
		 *	music=path/to/musictrack.ogg,Name
         * @param trackListFile The opened track list file.
         */
		void LoadAudioList(const std::string& trackListFile);
		
		/**
		 * Open and play an Audio Stream.
		 * This is essentially a short cut to opening and setting up a stream.
		 * This method will set the AudioStream into a loop and set any play
		 * options as specified. If you use OpenStream() yourself you won't be
		 * able to take advantage of fading already playing tracks.
         * @param name Name of the track. The name is specified in the track list, if
		 * the track isn't found it will attempt to be opened as if the name specifies
		 * a file. Keep in mind that using the name+list mapping abstracts what files
		 * are to be played, this means that you can modify what file corresponds to
		 * a specific name and the code will not need modifying. This is particularly
		 * useful for sound effects.
         * @param currentTrackOptions What to do with currently playing tracks.
         * @param inOptions How to play the specified track.
         * @return If the track to be played a pointer to an AudioStream, otherwise
		 * a null pointer if there was an error trying to play the track.
         */
		shared_ptr<AudioStream> PlayMusic(	const std::string& name,
											CurrentTrackOption currentTrackOptions = CurrentTrackOptions::FADE_ALL,
											PlayOption inOptions=PlayOptions::FADE_IN);

		/**
		 * Add a named file as the music list.
         * @param referenceName name to reference the track.
         * @param fileName The path and file name of the file to play.
         */
		void AddMusic(const std::string& referenceName, const std::string& fileName);

		/**
		 * Stop all of the music tracks currently playing.
         * @param options How to stop the tracks.
         */
		void StopMusic(CurrentTrackOption options=CurrentTrackOptions::FADE_ALL);

		/**
		 * Get a pointer to the specified track.
         * @param name The name of the track.
         * @return If the music track is loaded this method returns a pointer to the
		 * AudioStream to control the track, otherwise a null pointer.
         */
		shared_ptr<AudioStream> GetMusic(const std::string& name);

		/**
		 * Add a named sound file as an available file.
         * @param referenceName Name which to refer to the sound as.
         * @param fileName The path and name of the file of the sound.
         * @param delayLoad If true the sound should be loaded now when played rather than
		 * upon calling this method.
         */
		void AddSound(const std::string& referenceName, std::string fileName, bool delayLoad=false);
		
		/**
		 * Manually load a sound by name.
		 * This does not play the sound.
         * @param referenceName The name of the sound.
         * @return a pointer to the audio buffer if the sound could be loaded, otherwise a
		 * null pointer.
         */
		shared_ptr<AudioBuffer> LoadSound(const std::string& referenceName);
		
		/**
		 * Play a sound.
         * @param name
         * @return 
         */
		shared_ptr<AudioBuffer> PlaySound(const std::string& name);

		/**
		 * Set the default music volume.
		 * This is the volume adopted by AudioStreams when they are played using PlayTrack().
		 * You can modify the volume of an AudioStream independently as well.
		 * @param volume The new default volume.
		 */
		void SetDefaultMusicVolume(f32 volume);

		/**
		 * Set the default sound volume.
		 * This is the volume adopted by sounds when they are played using PlaySound().
		 * You can modify the volume of an AudioBuffer independently as well.
		 * @param volume The new default volume.
		 */
		void SetDefaultSoundVolume(f32 volume);

		/**
		 * Get the default music volume.
         * @return The default music volume.
         */
		f32 GetDefaultMusicVolume() const {return mMusicVolume;}

		/**
		 * Get the default sound volume.
         * @return The default sound volume.
         */
		f32 GetDefaultSoundVolume() const {return mSoundVolume;}

		/**
		 * During update the volumes of the playing music tracks are modified.
         */
		void Update(Seconds lastFrameTime) override;

		/**
		 * Add a loader function to load files with a given extension.
		 * This method will convert the extension to lowercase for comparison. This is
		 * because some applications save files with a capitalised extension and others
		 * lowercase. For example "WAV" vs "wav".
         * @param loader The loader function object.
         * @param extension The extension without the '.' character, for example "wav".
         */
		void AddLoader(AudioSourceLoader loader, const std::string& extension);

		/**
		 * Create a loader of a specific type.
		 * This just registers a loader with AddLoader but uses an internal templated
		 * create method if the audio source can take a file object as a constructor
		 * parameter.
         * @param extension The extension without the '.' character, for example "wav".
         */
		template< typename T >
		void CreateLoader(const std::string& extension)
		{
			AddLoader(bind(&AudioPlayer::CreateAudioSource<T>,this,placeholders::_1),extension);
		}
		
		/**
		 * Remove the loader for the given extension.
         * @param extension The extension without the '.' character, for example "wav".
		 */
		void RemoveLoader(const std::string& extension);

		/**
		 * Remove all loaders.
         */
		void RemoveAllLoaders();
	private:
		shared_ptr<AudioSource> CreateSource(const std::string& fileName);
		
		template<typename T>
		shared_ptr<AudioSource> CreateAudioSource(File file)
		{
			return shared_ptr<AudioSource>(new T(file));
		}
		
		shared_ptr<Audio> mAudio;
		shared_ptr<FileSystem> mFileSystem;
				
		std::map< std::string, AudioSourceLoader > mAudioSourceLoaders;
		
		std::map< std::string, std::string > mTrackList;
		std::map< std::string, shared_ptr<AudioStream> > mLoadedTracks;

		std::map< std::string, std::string > mSoundList;
		std::map< std::string, shared_ptr<AudioBuffer> > mLoadedSounds;

		std::list< shared_ptr<AudioStream> > mFadeOutBuffers;
		std::list< shared_ptr<AudioStream> > mFadeInBuffers;
		std::list< shared_ptr<AudioStream> > mCurrentTracks;

		f32 mMusicVolume;
		f32 mMinMusicVolume;
		f32 mSoundVolume;
	};
}
#endif
