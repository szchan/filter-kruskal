#pragma once

#include "utils.h"

// the maximum number of buckets for this implementation is 256 = 2^8
constexpr size_t logBuckets = 8;
constexpr size_t numBuckets = 1 << logBuckets;
constexpr size_t numPivots = numBuckets - 1;

// calculates the sample size based on an oversampling coefficient
ISize calcSampleSize(ISize n, ISize k) {
	double r = sqrt(double(n) / (2 * numBuckets * (logBuckets + 4)));
	return ISize(k * max(r, 1.0));
}

// extracts a sample from the list 
// and puts it in the last sampleSize elements of the list
void sample(vector<Edge> &edges, ISize begin, ISize end, ISize sampleSize) {
	static Random rnd(31);

	for (ISize i = 0; i < sampleSize; i++) {
		ISize index = rnd.getULong(begin, end);
		std::swap(edges[--end], edges[index]);
	}
}

// this works if beginSamples + endSamples doesn't overflow 
// AND if edges.size() >= (sampleSize + numPivots);
void buildTree(vector<Edge> &edges, ISize beginEdges, ISize beginSamples, ISize endSamples, vector<int> &pivots, int pos) {
	ISize mid = (beginSamples + endSamples) / 2;
	pivots[pos] = edges[mid].w;
	swap(edges[mid], edges[beginEdges]);
	if (2 * pos < numPivots) {
		buildTree(edges, 2*beginEdges, beginSamples, mid, pivots, 2*pos);
		buildTree(edges, 2*beginEdges+1, mid+1, endSamples, pivots, 2*pos+1);
	}
}

// builds the binary search tree used to identify the buckets
// the first pivot is at position 1 in the array
void buildTree(vector<Edge> &edges, ISize begin, ISize end, vector<int> &pivots, int sampleSize) {
	buildTree(edges, begin, end - sampleSize, end, pivots, 1);
}

// returns the bucket id of the specified element
// if the element is equal to the pivot it goes to the left
// the number of buckets cannot be greater than 256
u8 findBucket(const vector<int> &pivots, int val) {
	u32 id = 1;
	for (int i = 0; i < logBuckets; i++) {
		id = 2 * id + (val > pivots[id]);
	}
	return u8(id - numBuckets); // in the paper it says id - k + 1, but this should be correct
}

// bucketSizes has size numBuckets and should be allocated for every layer
// bucketIds has size edges.size() and should be allocated once
void assignBuckets(vector<Edge> &edges, ISize begin, ISize end, const vector<int> &pivots, vector<ISize> &bucketSizes, vector<u8> &bucketIds) {
	for (ISize i = begin; i < end; i++) {
		Edge &edge = edges[i];
		u8 bid = findBucket(pivots, edge.w);
		++bucketSizes[bid];
		bucketIds[i] = bid;
	}
}

// calculates the offset of every bucket starting from the start of the edges array
void prefixSum(vector<ISize> &bucketSizes, ISize begin) {
	ISize sum = begin;
	for (int i = 0; i < numBuckets; i++) {
		ISize curr = bucketSizes[i];
		bucketSizes[i] = sum;
		sum += curr;
	}
}

// puts every edge into the respective bucket
void distribute(vector<Edge> &edges, vector<Edge> &outEdges, ISize begin, ISize end, vector<ISize> &bucketSizes, vector<u8> &bucketIds) {
	for (ISize i = begin; i < end; i++) {
		outEdges[bucketSizes[bucketIds[i]]++] = edges[i];
	}
}

u64 sampleKruskal(DisjointSet &set, vector<Edge> &edges, vector<Edge> &outEdges, ISize begin, ISize end, vector<u8> &bucketIds) {
	
}

// partition in 2^k partitions
vector<vector<Edge>> kWayPartitioning(DisjointSet &set, vector<Edge> &edges, vector<Edge> &outEdges, const vector<int> &pivots, bool doFilter) {
	vector<vector<Edge>> partitions(pivots.size()+1);
}