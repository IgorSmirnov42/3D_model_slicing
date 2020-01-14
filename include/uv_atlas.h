#pragma once

#include <libmesh/debug/progress.h>
#include <libmesh/atlas/chart.h>

#include <libuvatlas/DirectXMesh/Meshconvert/uv_atlas_mesh.h>

namespace geom {

namespace details {
	struct ProgressCallback {
		ProgressCallback(ProgressLogger *logger);
		HRESULT operator()(float fPercentDone);
		ProgressLogger *logger;
	private:
		float max_value_;
	};

	class ProgressCallbackMultiplexer {
	public:
		class MultiplexedProgressCallback {
		public:
			MultiplexedProgressCallback(ProgressCallbackMultiplexer *multiplexer, int id)
					: multiplexer_(multiplexer), id_(id)
			{}

			HRESULT operator()(float fPercentDone)
			{
				multiplexer_->updateProgress(id_, fPercentDone);
				return S_OK;
			}
		private:
			ProgressCallbackMultiplexer *multiplexer_;
			int id_;
		};

		explicit ProgressCallbackMultiplexer(ProgressCallback &callback)
				:  progress_callback_(callback), total_progress_(0), total_weight_(0)
		{

		}

		MultiplexedProgressCallback multiplex(float weight)
		{
			int id = progresses_.size();
			progresses_.push_back(0);
			weights_.push_back(weight);
			total_weight_ += weight;
			return MultiplexedProgressCallback(this, id);
		}
	private:
		void updateProgress(int id, float progress)
		{
			total_progress_ -= progresses_[id] * weights_[id];
			total_progress_ += progress * weights_[id];
			progresses_[id] = progress;
			progress_callback_(total_progress_);
		}

		ProgressCallback &progress_callback_;
		double total_weight_;
		double total_progress_;
		std::vector<float> progresses_;
		std::vector<float> weights_;
	};
}

class UVAtlasGenerator {
public :
	UVAtlasGenerator();

	void append(sh_ptr_chart &chart);
	bool apply();
	template <typename MeshType>
	void getMesh(MeshType *mesh) {
		std::cout << "Retrieving mesh" << std::endl;
		size_t nfaces		= uv_mesh_->GetFaceCount();
		size_t nvertices	= uv_mesh_->GetVertexCount();

		mesh->resize_faces(nfaces);
		mesh->resize_vertices(nvertices);

		const auto in_indexBuffer    = uv_mesh_->GetIndexBuffer();
		const auto in_texCoordBuffer = uv_mesh_->GetTexCoordBuffer();

		typename MeshType::FaceContainer   &	out_faces		= mesh->faces();
		typename MeshType::VertexContainer &	out_vertices	= mesh->vertices();

		for (size_t i = 0; i < nvertices; ++i) {
			typename MeshType::Vertex vertex;
			vertex.p = vector2f(in_texCoordBuffer[i].x, in_texCoordBuffer[i].y);
			std::cout << vertex.p << std::endl;
			out_vertices[i] = vertex;

		}

		std::vector<size_t> face_offsets;
		size_t total_faces = 0;

		for (const auto &chart : charts_) {
			face_offsets.push_back(total_faces);
			total_faces += chart->size_faces();
		}

		for (auto chart_idx = 0; chart_idx < charts_.size(); ++chart_idx) {
			const auto &chart = charts_[chart_idx];

			for (size_t i = 0; i < chart->size_faces(); ++i) {
				auto base_face_idx = chart->faces()[i].base;
				index3u face;
				face[0] = in_indexBuffer[3 * (face_offsets[chart_idx] + i) + 0];
				face[1] = in_indexBuffer[3 * (face_offsets[chart_idx] + i) + 1];
				face[2] = in_indexBuffer[3 * (face_offsets[chart_idx] + i) + 2];

				out_faces[base_face_idx].v = face;
				out_faces[base_face_idx].id = face2Page_partitioning_[face_offsets[chart_idx] + i];

			}
		}
	}

	void setPageCount(size_t npages);
	void setFaceDetalization(const std::vector<float> detalization);
	void setProgress(ProgressLogger *progress);

private:
	std::unique_ptr<details::ProgressCallback> progressCallback_;
	ProgressLogger *		progress_;

	std::vector<sh_ptr_chart> charts_;
	std::unique_ptr<UVAtlasMesh> uv_mesh_;

	size_t npages_;

	std::vector<size_t> face2Page_partitioning_;
	std::vector<float> IMT_;
};

}
