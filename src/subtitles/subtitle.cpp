#include "subtitles/subtitle.h"
#include <QThread>

Subtitle::Subtitle(AVStream* stream, std::shared_ptr<AVFormatContext> frm_ctx) :
        IMediaContext(stream, 1),
        format_context(frm_ctx),  // 1 = one thread required in subtitles decoding
        demuxer(new Demuxer(frm_ctx.get(), this)) {
    connect(this, &IMediaContext::endReached, [this] {
        decoder.drain_decoder();
        emit newPacketArrived();
    });
}

void Subtitle::connect_all(SubtitlesOutputer* outputer_, Clock* clock) {
    outputer = outputer_;
    demuxer->add_context(this);
    connect(outputer, &SubtitlesOutputer::requestSubtitles, this, &Subtitle::process_packet, Qt::QueuedConnection);
    connect(
        clock,
        &Clock::timeChanged,
        this,
        [this, clock]() {
            clear();
            outputer->stop_and_clear();
            qint64 newtime_us = clock->get_time() * 1'000'000.0;
            demuxer->seek(newtime_us);
        },
        Qt::QueuedConnection);
    emit outputer->requestSubtitles();
}
void Subtitle::disconnect_and_clear() {
    this->disconnect();
    demuxer->remove_context(this);
    clear();
}
void Subtitle::process_packet() {
    std::lock_guard _(mutex);
    if (!outputer || outputer->subs_queue.is_full())
        return;
    Packet packet = packet_queue.try_pop();
    if (!decoder.is_drained())
        emit requestPacket();
    SubtitleUnit sub = decoder.decode_subtitle(packet);
    if (sub.text.isEmpty())
        return;
    outputer->subs_queue.push(std::move(sub));
    emit outputer->outputSubtitles();
}
qint64 Subtitle::buffer_available() { return 0; }

void Subtitle::clear() {
    std::lock_guard _(mutex);
    packet_queue.clear();
    decoder.clear_decoder();
}
Subtitle::~Subtitle() {}