@0xb26a2bd6454ed96d;
using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("Echo::Resources");

struct MappedInputDevice
{
	struct AnalogueInput
	{
		name @0 :Text;				# The name of the input.
		source @1 :List(Text);		# The input that will map to this input
		value @2 :List(Float32);	# If source is a digital input the value corresponds to the the analogue value when the digital input is activated.
		# The first source determines the type of transform the mapping will have.
		# Only a single source can be used if it is an analogue to analogue mapping and value is ignored.
		# Multiple digital sources can be specified. When the digital inputs are pressed the analogue input takes the value of 1 unless a value has been specified.
	}
	struct DigitalInput
	{
		name @0 :Text;			# The name of the input
		source @1 :Text;		# The input source that will map to this input. Digital to digital mappings can be made as well as analogue to digital mappings.
		threshold @2 :Float32;	# For analogue inputs, the activate threshold. If not specified a value of 0.5 is used. The value can be negative.
	}
	name @0 :Text;						# Name of the input device.
	analogue @1 :List(AnalogueInput);	# Repeated list of analogue input mappings
	digital @2 :List(DigitalInput);		# Repeated list of digital input mappings
}
