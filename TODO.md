# Project TODO: List

## Week 1 Feedback Improvements
- [x] Refactor Command Parser using a Command Table (Done)
- [x] Implement strict syntax validation (Done)
- [x] Enhance 'show' command to support specific channel IDs

## Unit Testing (10 Scenarios)
- [ ] Scenario 1: Strict space validation (add 1  1,2) -> Error
- [ ] Scenario 2: Command prefix validation (address 1 1,2) -> Unknown
- [ ] Scenario 3: Full capacity test (add 1 1-80)
- [ ] Scenario 4: Overlap/Conflict test (add 1 10-20, then add 2 15-25)
- [ ] Scenario 5: Metadata update on head removal (rmv first slot)
- [ ] Scenario 6: Boundary check (ID 0, ID 100)
- [ ] Scenario 7: Continuous range re-configuration
- [ ] Scenario 8: Invalid slot format (add 1 1, 2)
- [ ] Scenario 9: Show empty state
- [ ] Scenario 10: State persistence after multiple add/rmv
