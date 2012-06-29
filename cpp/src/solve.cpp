#include <cstdio>
#include <vector>
#include "ap.h"
#include "dataanalysis.h"
using namespace std;
using namespace alglib;

#define all(c) (c).begin(), (c).end()
#define iter(c) __typeof((c).begin())
#define cpresent(c, e) (find(all(c), (e)) != (c).end())
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define tr(c, i) for (iter(c) i = (c).begin(); i != (c).end(); ++i)
#define pb(e) push_back(e)
#define mp(a, b) make_pair(a, b)

extern const char *dfdump_050_1[];

void usage(const char *prog) {
  printf("%s train target\n", prog);
  exit(-1);
}

int main(int argc, char *argv[]) {
  decisionforest df; string dfs;
  for (int i=0; dfdump_050_1[i]; ++i) dfs.append(dfdump_050_1[i]);
  dfunserialize(dfs, df);
  if (argc != 3) usage(argv[0]);
  FILE *fp;
  if ((fp = fopen(argv[2], "r")) == NULL) {
    fprintf(stderr, "cannot open %s\n", argv[2]);
    return -1;
  }
  int v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17;
  while (fscanf(fp, " %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9, &v10, &v11, &v12, &v13, &v14, &v15, &v16, &v17) == 17) {
    if (v1 < 14) {
      printf("0\n");
      continue;
    }
    vector<double> sample;
    #define nominal(C,V) sample.pb((C)==(V) ? 1.0 : 0.0)
    sample.pb((double) v1);     // Age
    nominal(v2, 2);             // Make:Female
    nominal(v3, 1);             // White
    nominal(v3, 2);             // Black/Gegro
    nominal(v3, 3);             // American Indian
    nominal(v3, 4);             // Chinese
    nominal(v3, 5);             // Japanese
    nominal(v3, 6);             // Other Asian or Pacific
    nominal(v3, 7);             // Other race, nec
    nominal(v4, 1);             // Married, spouse present
    nominal(v4, 2);             // Married, spouse absent
    nominal(v4, 3);             // Separated
    nominal(v4, 4);             // Divorced
    nominal(v4, 5);             // Widowed
    nominal(v4, 6);             // Never married/single
    sample.pb((double) v5);     // # of children
    nominal(v6, 1);             // Alabama
    nominal(v6, 2);             // Alaska
    nominal(v6, 4);             // Arizona
    nominal(v6, 5);             // Arkansas
    nominal(v6, 6);             // California
    nominal(v6, 8);             // Colorado
    nominal(v6, 9);             // Conneticut
    nominal(v6, 10);            // Delaware
    nominal(v6, 11);            // District of Columbia
    nominal(v6, 12);            // Florida
    nominal(v6, 13);            // Geogia
    nominal(v6, 15);            // Hawaii
    nominal(v6, 16);            // Idaho
    nominal(v6, 17);            // Illinois
    nominal(v6, 18);            // Indiana
    nominal(v6, 19);            // Iowa
    nominal(v6, 20);            // Kansas
    nominal(v6, 21);            // Kentucky
    nominal(v6, 22);            // Louisiana
    nominal(v6, 23);            // Maine
    nominal(v6, 24);            // Maryland
    nominal(v6, 25);            // Massachusetts
    nominal(v6, 26);            // Michigan
    nominal(v6, 27);            // Minnesota
    nominal(v6, 28);            // Mississippi
    nominal(v6, 29);            // Missouri
    nominal(v6, 30);            // Montana
    nominal(v6, 31);            // Nebraska
    nominal(v6, 32);            // Nevada
    nominal(v6, 33);            // New Hampshire
    nominal(v6, 34);            // New Jersey
    nominal(v6, 35);            // New Mexico
    nominal(v6, 36);            // New York
    nominal(v6, 37);            // North Carolina
    nominal(v6, 38);            // North Dakota
    nominal(v6, 39);            // Ohio
    nominal(v6, 40);            // Oklahoma
    nominal(v6, 41);            // Oregon
    nominal(v6, 42);            // Pennsylvania
    nominal(v6, 44);            // Rhode Island
    nominal(v6, 45);            // South Carolina
    nominal(v6, 46);            // South Dakota
    nominal(v6, 47);            // Tennessee
    nominal(v6, 48);            // Texas
    nominal(v6, 49);            // Utah
    nominal(v6, 50);            // Vermont
    nominal(v6, 51);            // Virginia
    nominal(v6, 53);            // Washington
    nominal(v6, 54);            // West Virginia
    nominal(v6, 55);            // Wyoming
    nominal(v6, 90);            // Native American
    nominal(v6, 99);            // United States, ns
    nominal(v6, 100);           // American Samoa
    nominal(v6, 105);           // Guam
    nominal(v6, 110);           // Puerto Rico
    nominal(v6, 115);           // U.S. Virgin Islands
    nominal(v6, 120);           // Other US Possesions
    nominal(v6, 150);           // Canada
    nominal(v6, 160);           // Atlantic Islands
    nominal(v6, 199);           // North America, ns
    nominal(v6, 200);           // Mexico
    nominal(v6, 210);           // Central America
    nominal(v6, 250);           // Cuba
    nominal(v6, 260);           // West Indies
    nominal(v6, 300);           // South America
    nominal(v6, 400);           // Denmark
    nominal(v6, 401);           // Finland
    nominal(v6, 402);           // Iceland
    nominal(v6, 403);           // Lapland
    nominal(v6, 404);           // Norway
    nominal(v6, 405);           // Sweden
    nominal(v6, 410);           // England
    nominal(v6, 411);           // Scotland
    nominal(v6, 412);           // Wales
    nominal(v6, 413);           // United Kingdom, ns
    nominal(v6, 414);           // Ireland
    nominal(v6, 419);           // Northern Europe, ns
    nominal(v6, 420);           // Belgium
    nominal(v6, 421);           // France
    nominal(v6, 422);           // Liechtenstein
    nominal(v6, 423);           // Luxembourg
    nominal(v6, 424);           // Monaco
    nominal(v6, 425);           // Netherlands
    nominal(v6, 426);           // Switerland
    nominal(v6, 429);           // Western Europe, ns
    nominal(v6, 430);           // Albania
    nominal(v6, 431);           // Andorra
    nominal(v6, 432);           // Gibraltar
    nominal(v6, 433);           // Greece
    nominal(v6, 434);           // Italy
    nominal(v6, 435);           // Malta
    nominal(v6, 436);           // Portugal
    nominal(v6, 437);           // San Marino
    nominal(v6, 438);           // Spain
    nominal(v6, 439);           // Vatican City
    nominal(v6, 440);           // Southern Europe, ns
    nominal(v6, 450);           // Austria
    nominal(v6, 451);           // Bulgaria
    nominal(v6, 452);           // Czechoslovakia
    nominal(v6, 453);           // Germany
    nominal(v6, 454);           // Hungary
    nominal(v6, 455);           // Poland
    nominal(v6, 456);           // Romania
    nominal(v6, 457);           // Yugoslavia
    nominal(v6, 458);           // Central Europe, ns
    nominal(v6, 459);           // Eastern Europe ns
    nominal(v6, 460);           // Estonia
    nominal(v6, 461);           // Latvia
    nominal(v6, 462);           // Lithuania
    nominal(v6, 463);           // Baltic States
    nominal(v6, 465);           // USSR/Russia
    nominal(v6, 499);           // Europe, ns
    nominal(v6, 500);           // China
    nominal(v6, 501);           // Japan
    nominal(v6, 502);           // Korea
    nominal(v6, 509);           // East Asia, ns
    nominal(v6, 510);           // Brunei
    nominal(v6, 511);           // Cambodia (Kampuchea)
    nominal(v6, 512);           // Indonesia
    nominal(v6, 513);           // Laos
    nominal(v6, 514);           // Malaysia
    nominal(v6, 515);           // Phillipines
    nominal(v6, 516);           // Singapore
    nominal(v6, 517);           // Thailand
    nominal(v6, 518);           // Vietnam
    nominal(v6, 519);           // Southeast Asia, ns
    nominal(v6, 520);           // Afganistan
    nominal(v6, 521);           // India
    nominal(v6, 522);           // Iran
    nominal(v6, 523);           // Maldives
    nominal(v6, 524);           // Nepal
    nominal(v6, 530);           // Bahrain
    nominal(v6, 531);           // Cyprus
    nominal(v6, 532);           // Iraq
    nominal(v6, 533);           // Iraq/Saudi Arabia
    nominal(v6, 534);           // Israel/Palestine
    nominal(v6, 535);           // Jordan
    nominal(v6, 536);           // Kuwait
    nominal(v6, 537);           // Lebanon
    nominal(v6, 538);           // Oman
    nominal(v6, 539);           // Qatar
    nominal(v6, 540);           // Saudi Arabia
    nominal(v6, 541);           // Syria
    nominal(v6, 542);           // Turkey
    nominal(v6, 543);           // United Arab Emirates
    nominal(v6, 544);           // Yemen Arab Republic (North)
    nominal(v6, 545);           // Yemen, PDR (South)
    nominal(v6, 546);           // Persian Gulf States, ns
    nominal(v6, 547);           // Middle East, ns
    nominal(v6, 548);           // Southwest Asia, nec/ns
    nominal(v6, 549);           // Asia Minor, ns
    nominal(v6, 550);           // SouthAsia, nec
    nominal(v6, 599);           // Asia, nec/ns
    nominal(v6, 600);           // Africa
    nominal(v6, 700);           // Australia and New Zealand
    nominal(v6, 710);           // Pacific Islands
    nominal(v6, 800);           // Antarctica, ns/nec
    nominal(v6, 900);           // Abroad (unknown) or at sea
    nominal(v6, 997);           // Missing/Unknown
    nominal(v6, 999);           // Unknown value
    //    nominal(v7, 2);             // Farmer
    nominal(v8, 0);             // N/A
    nominal(v8, 1);             // Owned or being bought (loan)
    nominal(v8, 2);             // Rented
    nominal(v9, 1);             // Head/Householder
    nominal(v9, 2);             // Spouse
    nominal(v9, 3);             // Child
    nominal(v9, 4);             // Child-in-law
    nominal(v9, 5);             // Parent
    nominal(v9, 6);             // Parent-in-Law
    nominal(v9, 7);             // Sibling
    nominal(v9, 8);             // Sibling-in-Law
    nominal(v9, 9);             // Grandchild
    nominal(v9, 10);            // Other relatives
    nominal(v9, 11);            // Partner, friend, visitor
    nominal(v9, 12);            // Other non-relatives
    nominal(v9, 13);            // Institutional inmates
    nominal(v10, 0);            // N/A
    nominal(v10, 1);            // No, not in school
    nominal(v10, 2);            // Yes, in school
    nominal(v11, 0);            // N/A
    nominal(v11, 1);            // Employed
    nominal(v11, 2);            // Unemployed
    nominal(v11, 3);            // Not in labor force
    nominal(v12, 0);            // N/A
    nominal(v12, 1);            // Self-employed
    nominal(v12, 2);            // Works for wages/salary
    nominal(v12, 3);            // New worker
    nominal(v12, 4);            // Unemployed, last worked 5 years ago
    sample.pb((double) v13);    // work weeks
    sample.pb((double) v14);    // work hours
    sample.pb((double) v15);    // worked since
    nominal(v16, 0);            // N/A
    nominal(v16, 1);            // Same house
    nominal(v16, 2);            // Moved, place not reported
    nominal(v16, 3);            // Same state/county, different house
    nominal(v16, 4);            // Same state, different county
    nominal(v16, 5);            // Different state
    nominal(v16, 6);            // Abroad
    nominal(v16, 7);            // Same state, place not reported
    nominal(v16, 9);            // Unknown
    nominal(v17, 0);            // N/A
    nominal(v17, 10);           // Auto, truck, or van
    nominal(v17, 11);           // Auto
    nominal(v17, 12);           // Driver
    nominal(v17, 13);           // Passenger
    nominal(v17, 14);           // Truck
    nominal(v17, 15);           // Van
    nominal(v17, 20);           // Motorcycle
    nominal(v17, 30);           // Bus or streetcar
    nominal(v17, 31);           // Bus or trolley bus
    nominal(v17, 32);           // Streetcar or trolley car
    nominal(v17, 33);           // Subway or elevated
    nominal(v17, 34);           // Railroad
    nominal(v17, 35);           // Taxicab
    nominal(v17, 36);           // Ferryboat
    nominal(v17, 40);           // Bicycle
    nominal(v17, 50);           // Walked only
    nominal(v17, 60);           // Other
    nominal(v17, 70);           // Worked at home
//    sample.pb((double) v18);    // Annual salary
    #undef nominal
    int cols = sample.size();
    real_1d_array x, y;
    x.setcontent(cols, &sample[0]);
    dfprocess(df, x, y);
    printf("%d\n", (int) ceil(y[0]));
  }
  fclose(fp);
  return 0;
}
