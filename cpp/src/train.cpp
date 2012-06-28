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

int main(int argc, char *argv[]) {
  FILE *fp;
  if (argc != 2 || (fp = fopen(argv[1], "r")) == NULL) return -1;
  int v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18;
  int rows=0;
  vector<double> sample;
  while (fscanf(fp, " %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9, &v10, &v11, &v12, &v13, &v14, &v15, &v16, &v17, &v18) == 18) {
    if (v1 < 14) continue;
    #define NOMINAL(C,V) sample.push_back((C)==(V) ? 1.0d : 0.0d)
    sample.push_back((double) v1); // Age
    NOMINAL(v2, 2);                // Make:Female
    NOMINAL(v3, 1);                // White
    NOMINAL(v3, 2);                // Black/Gegro
    NOMINAL(v3, 3);                // American Indian
    NOMINAL(v3, 4);                // Chinese
    NOMINAL(v3, 5);                // Japanese
    NOMINAL(v3, 6);                // Other Asian or Pacific
    NOMINAL(v3, 7);                // Other race, nec
    NOMINAL(v4, 1);                // Married, spouse present
    NOMINAL(v4, 2);                // Married, spouse absent
    NOMINAL(v4, 3);                // Separated
    NOMINAL(v4, 4);                // Divorced
    NOMINAL(v4, 5);                // Widowed
    NOMINAL(v4, 6);                // Never married/single
    sample.push_back((double) v5); // # of children
    NOMINAL(v6, 1);                // Alabama
    NOMINAL(v6, 2);                // Alaska
    NOMINAL(v6, 4);                // Arizona
    NOMINAL(v6, 5);                // Arkansas
    NOMINAL(v6, 6);                // California
    NOMINAL(v6, 8);                // Colorado
    NOMINAL(v6, 9);                // Conneticut
    NOMINAL(v6, 10);               // Delaware
    NOMINAL(v6, 11);               // District of Columbia
    NOMINAL(v6, 12);               // Florida
    NOMINAL(v6, 13);               // Geogia
    NOMINAL(v6, 15);               // Hawaii
    NOMINAL(v6, 16);               // Idaho
    NOMINAL(v6, 17);               // Illinois
    NOMINAL(v6, 18);               // Indiana
    NOMINAL(v6, 19);               // Iowa
    NOMINAL(v6, 20);               // Kansas
    NOMINAL(v6, 21);               // Kentucky
    NOMINAL(v6, 22);               // Louisiana
    NOMINAL(v6, 23);               // Maine
    NOMINAL(v6, 24);               // Maryland
    NOMINAL(v6, 25);               // Massachusetts
    NOMINAL(v6, 26);               // Michigan
    NOMINAL(v6, 27);               // Minnesota
    NOMINAL(v6, 28);               // Mississippi
    NOMINAL(v6, 29);               // Missouri
    NOMINAL(v6, 30);               // Montana
    NOMINAL(v6, 31);               // Nebraska
    NOMINAL(v6, 32);               // Nevada
    NOMINAL(v6, 33);               // New Hampshire
    NOMINAL(v6, 34);               // New Jersey
    NOMINAL(v6, 35);               // New Mexico
    NOMINAL(v6, 36);               // New York
    NOMINAL(v6, 37);               // North Carolina
    NOMINAL(v6, 38);               // North Dakota
    NOMINAL(v6, 39);               // Ohio
    NOMINAL(v6, 40);               // Oklahoma
    NOMINAL(v6, 41);               // Oregon
    NOMINAL(v6, 42);               // Pennsylvania
    NOMINAL(v6, 44);               // Rhode Island
    NOMINAL(v6, 45);               // South Carolina
    NOMINAL(v6, 46);               // South Dakota
    NOMINAL(v6, 47);               // Tennessee
    NOMINAL(v6, 48);               // Texas
    NOMINAL(v6, 49);               // Utah
    NOMINAL(v6, 50);               // Vermont
    NOMINAL(v6, 51);               // Virginia
    NOMINAL(v6, 53);               // Washington
    NOMINAL(v6, 54);               // West Virginia
    NOMINAL(v6, 55);               // Wyoming
    NOMINAL(v6, 90);               // Native American
    NOMINAL(v6, 99);               // United States, ns
    NOMINAL(v6, 100);              // American Samoa
    NOMINAL(v6, 105);              // Guam
    NOMINAL(v6, 110);              // Puerto Rico
    NOMINAL(v6, 115);              // U.S. Virgin Islands
    NOMINAL(v6, 120);              // Other US Possesions
    NOMINAL(v6, 150);              // Canada
    NOMINAL(v6, 160);              // Atlantic Islands
    NOMINAL(v6, 199);              // North America, ns
    NOMINAL(v6, 200);              // Mexico
    NOMINAL(v6, 210);              // Central America
    NOMINAL(v6, 250);              // Cuba
    NOMINAL(v6, 260);              // West Indies
    NOMINAL(v6, 300);              // South America
    NOMINAL(v6, 400);              // Denmark
    NOMINAL(v6, 401);              // Finland
    NOMINAL(v6, 402);              // Iceland
    NOMINAL(v6, 403);              // Lapland
    NOMINAL(v6, 404);              // Norway
    NOMINAL(v6, 405);              // Sweden
    NOMINAL(v6, 410);              // England
    NOMINAL(v6, 411);              // Scotland
    NOMINAL(v6, 412);              // Wales
    NOMINAL(v6, 413);              // United Kingdom, ns
    NOMINAL(v6, 414);              // Ireland
    NOMINAL(v6, 419);              // Northern Europe, ns
    NOMINAL(v6, 420);              // Belgium
    NOMINAL(v6, 421);              // France
    NOMINAL(v6, 422);              // Liechtenstein
    NOMINAL(v6, 423);              // Luxembourg
    NOMINAL(v6, 424);              // Monaco
    NOMINAL(v6, 425);              // Netherlands
    NOMINAL(v6, 426);              // Switerland
    NOMINAL(v6, 429);              // Western Europe, ns
    NOMINAL(v6, 430);              // Albania
    NOMINAL(v6, 431);              // Andorra
    NOMINAL(v6, 432);              // Gibraltar
    NOMINAL(v6, 433);              // Greece
    NOMINAL(v6, 434);              // Italy
    NOMINAL(v6, 435);              // Malta
    NOMINAL(v6, 436);              // Portugal
    NOMINAL(v6, 437);              // San Marino
    NOMINAL(v6, 438);              // Spain
    NOMINAL(v6, 439);              // Vatican City
    NOMINAL(v6, 440);              // Southern Europe, ns
    NOMINAL(v6, 450);              // Austria
    NOMINAL(v6, 451);              // Bulgaria
    NOMINAL(v6, 452);              // Czechoslovakia
    NOMINAL(v6, 453);              // Germany
    NOMINAL(v6, 454);              // Hungary
    NOMINAL(v6, 455);              // Poland
    NOMINAL(v6, 456);              // Romania
    NOMINAL(v6, 457);              // Yugoslavia
    NOMINAL(v6, 458);              // Central Europe, ns
    NOMINAL(v6, 459);              // Eastern Europe ns
    NOMINAL(v6, 460);              // Estonia
    NOMINAL(v6, 461);              // Latvia
    NOMINAL(v6, 462);              // Lithuania
    NOMINAL(v6, 463);              // Baltic States
    NOMINAL(v6, 465);              // USSR/Russia
    NOMINAL(v6, 499);              // Europe, ns
    NOMINAL(v6, 500);              // China
    NOMINAL(v6, 501);              // Japan
    NOMINAL(v6, 502);              // Korea
    NOMINAL(v6, 509);              // East Asia, ns
    NOMINAL(v6, 510);              // Brunei
    NOMINAL(v6, 511);              // Cambodia (Kampuchea)
    NOMINAL(v6, 512);              // Indonesia
    NOMINAL(v6, 513);              // Laos
    NOMINAL(v6, 514);              // Malaysia
    NOMINAL(v6, 515);              // Phillipines
    NOMINAL(v6, 516);              // Singapore
    NOMINAL(v6, 517);              // Thailand
    NOMINAL(v6, 518);              // Vietnam
    NOMINAL(v6, 519);              // Southeast Asia, ns
    NOMINAL(v6, 520);              // Afganistan
    NOMINAL(v6, 521);              // India
    NOMINAL(v6, 522);              // Iran
    NOMINAL(v6, 523);              // Maldives
    NOMINAL(v6, 524);              // Nepal
    NOMINAL(v6, 530);              // Bahrain
    NOMINAL(v6, 531);              // Cyprus
    NOMINAL(v6, 532);              // Iraq
    NOMINAL(v6, 533);              // Iraq/Saudi Arabia
    NOMINAL(v6, 534);              // Israel/Palestine
    NOMINAL(v6, 535);              // Jordan
    NOMINAL(v6, 536);              // Kuwait
    NOMINAL(v6, 537);              // Lebanon
    NOMINAL(v6, 538);              // Oman
    NOMINAL(v6, 539);              // Qatar
    NOMINAL(v6, 540);              // Saudi Arabia
    NOMINAL(v6, 541);              // Syria
    NOMINAL(v6, 542);              // Turkey
    NOMINAL(v6, 543);              // United Arab Emirates
    NOMINAL(v6, 544);              // Yemen Arab Republic (North)
    NOMINAL(v6, 545);              // Yemen, PDR (South)
    NOMINAL(v6, 546);              // Persian Gulf States, ns
    NOMINAL(v6, 547);              // Middle East, ns
    NOMINAL(v6, 548);              // Southwest Asia, nec/ns
    NOMINAL(v6, 549);              // Asia Minor, ns
    NOMINAL(v6, 550);              // SouthAsia, nec
    NOMINAL(v6, 599);              // Asia, nec/ns
    NOMINAL(v6, 600);              // Africa
    NOMINAL(v6, 700);              // Australia and New Zealand
    NOMINAL(v6, 710);              // Pacific Islands
    NOMINAL(v6, 800);              // Antarctica, ns/nec
    NOMINAL(v6, 900);              // Abroad (unknown) or at sea
    NOMINAL(v6, 997);              // Missing/Unknown
    NOMINAL(v6, 999);              // Unknown value
    //    NOMINAL(v7, 2);                // Farmer
    NOMINAL(v8, 0);                // N/A
    NOMINAL(v8, 1);                // Owned or being bought (loan)
    NOMINAL(v8, 2);                // Rented
    NOMINAL(v9, 1);                // Head/Householder
    NOMINAL(v9, 2);                // Spouse
    NOMINAL(v9, 3);                // Child
    NOMINAL(v9, 4);                // Child-in-law
    NOMINAL(v9, 5);                // Parent
    NOMINAL(v9, 6);                // Parent-in-Law
    NOMINAL(v9, 7);                // Sibling
    NOMINAL(v9, 8);                // Sibling-in-Law
    NOMINAL(v9, 9);                // Grandchild
    NOMINAL(v9, 10);               // Other relatives
    NOMINAL(v9, 11);               // Partner, friend, visitor
    NOMINAL(v9, 12);               // Other non-relatives
    NOMINAL(v9, 13);               // Institutional inmates
    NOMINAL(v10, 0);               // N/A
    NOMINAL(v10, 1);               // No, not in school
    NOMINAL(v10, 2);               // Yes, in school
    NOMINAL(v11, 0);               // N/A
    NOMINAL(v11, 1);               // Employed
    NOMINAL(v11, 2);               // Unemployed
    NOMINAL(v11, 3);               // Not in labor force
    NOMINAL(v12, 0);               // N/A
    NOMINAL(v12, 1);               // Self-employed
    NOMINAL(v12, 2);               // Works for wages/salary
    NOMINAL(v12, 3);               // New worker
    NOMINAL(v12, 4);               // Unemployed, last worked 5 years ago
    sample.push_back((double) v13);// work weeks
    sample.push_back((double) v14);// work hours
    sample.push_back((double) v15);// worked since
    NOMINAL(v16, 0);               // N/A
    NOMINAL(v16, 1);               // Same house
    NOMINAL(v16, 2);               // Moved, place not reported
    NOMINAL(v16, 3);               // Same state/county, different house
    NOMINAL(v16, 4);               // Same state, different county
    NOMINAL(v16, 5);               // Different state
    NOMINAL(v16, 6);               // Abroad
    NOMINAL(v16, 7);               // Same state, place not reported
    NOMINAL(v16, 9);               // Unknown
    NOMINAL(v17, 0);               // N/A
    NOMINAL(v17, 10);              // Auto, truck, or van
    NOMINAL(v17, 11);              // Auto
    NOMINAL(v17, 12);              // Driver
    NOMINAL(v17, 13);              // Passenger
    NOMINAL(v17, 14);              // Truck
    NOMINAL(v17, 15);              // Van
    NOMINAL(v17, 20);              // Motorcycle
    NOMINAL(v17, 30);              // Bus or streetcar
    NOMINAL(v17, 31);              // Bus or trolley bus
    NOMINAL(v17, 32);              // Streetcar or trolley car
    NOMINAL(v17, 33);              // Subway or elevated
    NOMINAL(v17, 34);              // Railroad
    NOMINAL(v17, 35);              // Taxicab
    NOMINAL(v17, 36);              // Ferryboat
    NOMINAL(v17, 40);              // Bicycle
    NOMINAL(v17, 50);              // Walked only
    NOMINAL(v17, 60);              // Other
    NOMINAL(v17, 70);              // Worked at home
    sample.push_back((double) v18);// Annual salary
    ++rows;
  }
  fclose(fp);
  int cols = sample.size() / rows;
  real_2d_array xy;
  xy.setcontent(rows, cols, &sample[0]);
  decisionforest df;
  dfreport rep;
  ae_int_t info;
  for (int n=50; n<=100; n++) {
    for (int r=1; r<=6; r++) {
      double r_ = 0.11d * r;
      dfbuildrandomdecisionforest(xy, rows, cols-1, 1, n, r_, info, df, rep);
      string dfs;
      dfserialize(df, dfs);
      char name[1024];
      sprintf(name, "dfdump-%03d-%d", n, r);
      fp = fopen(name, "w");
      fprintf(fp, "%s\n\n", dfs.c_str());
      fclose(fp);
    }
  }
  return 0;
}
