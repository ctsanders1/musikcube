//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007-2017 musikcube team
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may
//      be used to endorse or promote products derived from this software
//      without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <core/config.h>
#include <core/library/track/Track.h>
#include <core/library/LocalLibrary.h>

namespace musik { namespace core {

    class IndexerTrack : public Track {
        public:
            IndexerTrack(int64_t id);
            virtual ~IndexerTrack(void);

            /* ITagStore */
            virtual void SetValue(const char* metakey, const char* value);
            virtual void ClearValue(const char* metakey);
            virtual bool Contains(const char* metakey);
            virtual void SetThumbnail(const char *data, long size);
            virtual void SetReplayGain(const musik::core::sdk::ReplayGain& replayGain);

            /* ITrack */
            virtual std::string GetString(const char* metakey);
            virtual int GetString(const char* key, char* dst, int size);
            virtual long long GetInt64(const char* key, long long defaultValue = 0LL);
            virtual int GetInt32(const char* key, unsigned int defaultValue = 0);
            virtual double GetDouble(const char* key, double defaultValue = 0.0f);

            virtual std::string Uri();
            virtual int Uri(char* dst, int size);

            virtual MetadataIteratorRange GetValues(const char* metakey);
            virtual MetadataIteratorRange GetAllValues();
            virtual TrackPtr Copy();

            virtual int64_t GetId();
            virtual void SetId(int64_t id) { this->id = id; }

            bool NeedsToBeIndexed(
                const boost::filesystem::path &file,
                db::Connection &dbConnection);

            bool Save(
                db::Connection &dbConnection,
                std::string libraryDirectory);

            static void ResetIdCache();

        protected:
            friend class Indexer;
            static std::mutex sharedWriteMutex;

        private:
            int64_t id;

        private:
            class InternalMetadata {
                public:
                    InternalMetadata();
                    ~InternalMetadata();

                    Track::MetadataMap metadata;
                    std::shared_ptr<musik::core::sdk::ReplayGain> replayGain;
                    char *thumbnailData;
                    int thumbnailSize;
            };

            InternalMetadata *internalMetadata;

            int64_t SaveThumbnail(
                db::Connection& connection,
                const std::string& libraryDirectory);

            int64_t SaveGenre(db::Connection& connection);

            int64_t SaveArtist(db::Connection& connection);

            int64_t SaveAlbum(db::Connection& connection, int64_t thumbnailId);

            int64_t SaveSingleValueField(
                db::Connection& connection,
                const std::string& trackMetadataKeyName,
                const std::string& fieldTableName);

            int64_t SaveMultiValueField(
                db::Connection& connection,
                const std::string& tracksTableColumnName,
                const std::string& fieldTableName,
                const std::string& junctionTableName,
                const std::string& junctionTableForeignKeyColumnName);

            int64_t SaveNormalizedFieldValue(
                db::Connection& dbConnection,
                const std::string& tableName,
                const std::string& fieldValue,
                bool isAggregatedValue,
                const std::string& relationJunctionTableName = "",
                const std::string& relationJunctionTableColumn = "");

            void SaveDirectory(
                db::Connection& dbConnection,
                const std::string& filename);

            void SaveReplayGain(db::Connection& dbConnection);

            void ProcessNonStandardMetadata(db::Connection& connection);
    };

} }
