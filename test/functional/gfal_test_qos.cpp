/*
 * Copyright (c) CERN 2013-2017
 *
 * Copyright (c) Members of the EMI Collaboration. 2010-2013
 *  See  http://www.eu-emi.eu/partners for details on the copyright
 *  holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <fcntl.h>
#include <stdio.h>
#include <gfal_api.h>
#include <stdlib.h>
#include <common/gfal_lib_test.h>
#include <common/gfal_gtest_asserts.h>
#include <utils/exceptions/gerror_to_cpp.h>

#define BLKLEN 65536


class QosTest: public testing::Test {
public:
    static const char* root;
    static const char *token;

    gfal2_context_t context;
    gfalt_params_t params;
    gfal2_cred_t *cred;

    QosTest() {
        GError *error = NULL;
        context = gfal2_context_new(&error);
        cred = gfal2_cred_new(GFAL_CRED_BEARER, token);
        Gfal::gerror_to_cpp(&error);
    }

    virtual ~QosTest() {
    	gfal2_cred_free(cred);
        gfal2_context_free(context);
    }

    virtual void SetUp() {
    	unsetenv("X509_USER_PROXY");
    	unsetenv("X509_USER_CERT");
    	unsetenv("X509_USER_KEY");

    	int ret;
    	GError *error = NULL;

    	Gfal::gerror_to_cpp(&error);
    	params = gfalt_params_handle_new(&error);
    	Gfal::gerror_to_cpp(&error);

    	ret = gfal2_cred_clean(context, &error);
    	ASSERT_PRED_FORMAT2(AssertGfalSuccess, ret, error);

    	// Clear automatic setup
    	gfal2_remove_opt(context, "X509", "CERT", NULL);
    	gfal2_remove_opt(context, "X509", "KEY", NULL);

    	// Set configured values
    	ret = gfal2_cred_set(context, "dcache-xdc.desy.de", cred, &error);
    	ASSERT_PRED_FORMAT2(AssertGfalSuccess, ret, error);
    }

    virtual void TearDown() {
        GError *error = NULL;
    }
};

const char *QosTest::root = NULL;
const char *QosTest::token = NULL;

TEST_F(QosTest, TestSimpleCase)
{
	GError *err = NULL;
	gfal2_qos_check_classes(context, root, "dataobject", &err);
	EXPECT_EQ(NULL, err);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);

    /*if (argc < 2) {
        printf("Missing base url\n");
        printf("\t%s [options] srm://host/base/path/\n", argv[0]);
        return 1;
    }*/

    /*QosTest::root = argv[1];*/
    QosTest::root = "https://dcache-xdc.desy.de:6443";
    QosTest::token = "eyJraWQiOiJyc2ExIiwiYWxnIjoiUlMyNTYifQ.eyJzdWIiOiJmZWE1ZTZlMi0wYjlmLTQwZjUtYjE5OC00YmI3YWU0YjIzNGEiLCJpc3MiOiJodHRwczpcL1wvaWFtLmV4dHJlbWUtZGF0YWNsb3VkLmV1XC8iLCJleHAiOjE1MzA2MjAxMTksImlhdCI6MTUzMDYxNjUxOSwianRpIjoiODU0ODdhNDQtZWRlNS00NzcxLTg0MTctYjJkMTM3YzAxNmExIn0.AH-epaG5325e8Sq4X5oEe9-otB6fB_-JDRr6A7D5lLu1AHqThgWdT7Elr9T7Q-T6-d7UYjhy0xuoS_ojavjMu6OPnss1YvDZyVj6gny_ihtjgI5h6VNe-x8Jqbj-hyAijuMLdNNRbnBf9NeZEnyFBrJmLQrJP3pJUMk83y6xmO4";
    return RUN_ALL_TESTS();
}
