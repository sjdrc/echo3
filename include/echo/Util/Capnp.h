#ifndef _ECHOCAPNP_H_
#define _ECHOCAPNP_H_

#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <capnp/compat/json.h>

namespace Echo
{
	// /**
	//  * Parse a protocol buffer.
	//  * This function will attempt to parse as binary data first for best performance. If parsing fails
	//  * it will drop back to text format parsing and log a message if it succeeds.
	//  * @param data The data as a string.
	//  * @param intoObject the Message object to receive the data.
	//  * @param fileName The filename from which you're loading. Used only for logging.
	//  * @param messageLocation The code location to include in logging.
	//  */
	// template< typename Message >
	// bool ParseProtobuf(const std::string& data, Message& intoObject, const std::string& fileName, const std::string& messageLocation)
	// {
	// 	try
	// 	{
	// 		if(!intoObject.ParseFromString(data))
	// 		{
	// 			// #ifndef BUILD_WITH_PROTOBUF_LITE
	// 			// if(!google::protobuf::TextFormat::ParseFromString(data, &intoObject))
	// 			// {
	// 			// 	return false;
	// 			// }
	// 			// ECHO_LOG_INFO("Info " << messageLocation << " - Parsed data in \"" << fileName << "\" as text format. Convert your data to binary for improved performance.");
	// 			// #else
	// 			ECHO_LOG_ERROR("Failed to load " << messageLocation << " \"" << fileName << "\". NOTE: This build does not support the text format.");
	// 			return false;
	// 			//#endif
	// 		}
	// 		return true;
	// 	}catch(...)
	// 	{
	// 		ECHO_LOG_ERROR("Failed to load " << messageLocation << " \"" << fileName << "\".");
	// 		return false;
	// 	}
	// }

	/**
	 * This message helper may contain a builder or a reader depending on how we had to read the file.
	 * CreateCapnpReaderFromData() will first attempt to read as a binary format for performance. If reading fails then the function
	 * attempts to read the data as JSON.
	 * 
	 */
	struct CapnpMessageHelper
	{
		shared_ptr<::capnp::MessageReader> mReader;
		shared_ptr<::capnp::MallocMessageBuilder> mBuilder;

		template< typename CapnpType >
		inline typename CapnpType::Reader GetReader()
		{
			if(mReader)
			{
				return mReader->getRoot<CapnpType>();
			}
			if(mBuilder)
			{
				return mBuilder->getRoot<CapnpType>().asReader();
			}
			throw std::exception();
		}
	};

	template< typename CapnpType >
	inline shared_ptr<CapnpMessageHelper> CreateCapnpReaderFromData(const std::string& data)
	{
		try
		{
			::kj::ArrayInputStream inputStream(kj::ArrayPtr<const ::capnp::byte>(reinterpret_cast<const ::capnp::byte*>(data.data()),data.length()));
			shared_ptr<CapnpMessageHelper> messageHelper=make_shared<CapnpMessageHelper>();
			messageHelper->mReader = shared_ptr<::capnp::MessageReader>(new ::capnp::PackedMessageReader(inputStream));;
			return messageHelper;
		}catch(...)
		{}
		try
		{
			ECHO_LOG_INFO("Attempting to read JSON");
			// If we can't read from a data buffer as a binary struct we'll attempt to load JSON.
			::capnp::JsonCodec json;
			shared_ptr<::capnp::MallocMessageBuilder> message = make_shared<::capnp::MallocMessageBuilder>();
			ECHO_LOG_INFO("\tCreated a message builder");
			shared_ptr<CapnpMessageHelper> messageHelper=make_shared<CapnpMessageHelper>();
			ECHO_LOG_INFO("\tCreated a helper");
			typename CapnpType::Builder builder = message->initRoot<CapnpType>();
			ECHO_LOG_INFO("\tCreated a builder" << typeid(CapnpType).name() << "\n" << data);
			json.decode(kj::ArrayPtr<const char>(data.c_str(),data.length()), builder);
			ECHO_LOG_INFO("\tDecoded into builder" << typeid(CapnpType).name());
			messageHelper->mBuilder = message;
			return messageHelper;
		}catch(kj::Exception e)
		{
			ECHO_LOG_ERROR("Failed to process JSON: " << e.getFile() << ":" << e.getLine() << ":" << e.getDescription().cStr());
			// Let the caller log messages.
			return nullptr;
		}
	}
	template< typename CapnpType >
	inline shared_ptr<CapnpMessageHelper> CreateCapnpReaderFromData(const std::string& fileName, FileSystem& fileSystem)
	{
		std::string wholeFile;
		File file = fileSystem.Open(fileName);
		if(!file.IsOpen())
		{
			ECHO_LOG_ERROR("Could not open \"" << fileName << "\"");
			return nullptr;
		}
		file.ReadFileIntoString(wholeFile);
		return CreateCapnpReaderFromData<CapnpType>(wholeFile);
	}

	template< typename ContainerType, typename CapnpListReader >
	inline void ReadCapnpList(ContainerType& container, CapnpListReader reader)
	{
		for(Size i = 0; i < reader.size(); ++i)
		{
			container.push_back(reader[i]);
		}
	}

	template< typename ContainerType, typename CapnpListReader >
	inline void ReadCapnpTextList(ContainerType& container, CapnpListReader reader)
	{
		for(Size i = 0; i < reader.size(); ++i)
		{
			container.push_back(reader[i].cStr());
		}
	}
}

#endif
