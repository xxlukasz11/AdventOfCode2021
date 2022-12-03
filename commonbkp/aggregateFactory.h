#pragma once

#include <vector>
#include <sstream>

namespace common {

template<typename AggregateType>
class AggregateFactory {
private:
	using StreamType = std::istringstream;

public:
	// Creates Aggregate from given line. Template arguments are fields of the aggregate
	template<typename... Args>
	AggregateType create(const std::string& line);

	template<typename First>
	AggregateType createAggregate(StreamType& stream);

	template<typename First, typename Second>
	AggregateType createAggregate(StreamType& stream);

	template<typename First, typename Second, typename Third>
	AggregateType createAggregate(StreamType& stream);

};

template<typename AggregateType>
template<typename... Args>
inline AggregateType AggregateFactory<AggregateType>::create(const std::string& line) {
	constexpr int parameterPackSize = sizeof...(Args);
	static_assert(parameterPackSize <= 3, "Maximum number of template arguments is 3");
	StreamType stream(line);
	AggregateType aggregate = createAggregate<Args...>(stream);
	return aggregate;
}

template<typename AggregateType>
template<typename First>
inline AggregateType AggregateFactory<AggregateType>::createAggregate(StreamType& stream) {
	First first;
	stream >> first;
	return AggregateType{ first };
}

template<typename AggregateType>
template<typename First, typename Second>
inline AggregateType AggregateFactory<AggregateType>::createAggregate(StreamType& stream) {
	First first;
	stream >> first;
	Second second;
	stream >> second;
	return AggregateType{ first, second };
}

template<typename AggregateType>
template<typename First, typename Second, typename Third>
inline AggregateType AggregateFactory<AggregateType>::createAggregate(StreamType& stream) {
	First first;
	stream >> first;
	Second second;
	stream >> second;
	Third third;
	stream >> third;
	return AggregateType{ first, second, third };
}

}