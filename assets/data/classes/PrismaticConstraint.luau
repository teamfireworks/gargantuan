const c, t, p = require("@scripts/classes").unpack()

return c({
	name = "PrismaticConstraint",
	extends = "SlidingBallConstraint",
	description = "	Constraint its attachments to slide along one axis but not rotate. ",
	since = "0.1.0",
})
